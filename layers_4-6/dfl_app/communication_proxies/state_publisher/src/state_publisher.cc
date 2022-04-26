/*
* (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>
#include <rti/config/Logger.hpp>
#include <rti/core/policy/CorePolicy.hpp>

#include "application.hh"
#include "state.hh"
#include "common.hh"
#include "http.hh"
#include "auth.hh"

#include <memory>
#include <stdexcept>
#include <string>

void
run_publisher_application(
    unsigned domain_id,
    std::string const &server_host,
    std::string const &server_port,
    std::string const &participation_password
)
{
#define TRY_OR_CONTINUE(smt) \
    try { \
        smt; \
    } catch ( ... ) { \
        rti::util::sleep(dds::core::Duration(1)); \
        continue; \
    }

    bool may_participate = false, writer_online = may_participate;
    bool server_online = false;

    std::string server_id, prev_server_id;
    std::string role = "publisher";

    while (!application::shutdown_requested)
        TRY_OR_CONTINUE({
            server_id = prev_server_id = Http::get(server_host, server_port, "id");
            server_online = true;
            break;
        });

    std::cout << "Connected to server {" << server_id << "}"
        << std::endl << std::endl << std::flush;

    dds::domain::qos::DomainParticipantQos participant_qos
        = dds::core::QosProvider::Default().participant_qos();

    unsigned max_user_data_len = participant_qos
        .policy<rti::core::policy::DomainParticipantResourceLimits>()
        .participant_user_data_max_length();

    if (participation_password.size() > max_user_data_len)
        throw std::runtime_error(
            "Participation password exceeds QoS user_data length of "
            + max_user_data_len
        );

    participant_qos << dds::core::policy::UserData(
        dds::core::ByteSeq(
            participation_password.begin(),
            participation_password.end()
        )
    );

    std::vector<rti::core::policy::Property::Entry> property_entries = {
        { "server_id", server_id },
        { "role", role }
    };

    participant_qos << rti::core::policy::Property(
        property_entries.begin(),
        property_entries.end(),
        true
    );

    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    dds::sub::Subscriber builtin_subscriber = dds::sub::builtin_subscriber(participant);
    auto authentication_listener = std::make_shared<AuthenticationListener>(
        participation_password,

        server_id,
        prev_server_id,
        role,

        may_participate
    );

    std::vector<dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>> participant_reader;
    dds::sub::find<dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>>(
        builtin_subscriber,
        dds::topic::participant_topic_name(),
        std::back_inserter(participant_reader)
    );

    dds::sub::qos::DataReaderQos builtin_reader_qos = participant_reader[0].qos();
    builtin_reader_qos << dds::core::policy::Reliability(
        dds::core::policy::ReliabilityKind::RELIABLE,
        dds::core::Duration(15)
    );

    participant_reader[0].qos(builtin_reader_qos);
    participant_reader[0].set_listener(authentication_listener);
    participant.enable();

    dds::topic::Topic<State> topic(participant, "Model State");

    dds::pub::Publisher publisher(participant);
    dds::pub::DataWriter<State> writer = nullptr;

    dds::core::Duration sleep_duration{1};
    std::string prev_commround{}, next_server_id;

    while (!application::shutdown_requested) {
        try {
            next_server_id = Http::get(server_host, server_port, "id");

            if (next_server_id != server_id) {
                std::cout << "Local server ID changed from {"
                    << server_id << "} to {" << next_server_id << "}"
                    << " [CURRENTLY UNSUPPORTED]"
                    << std::endl << std::flush;

                prev_server_id = server_id;
                server_id = next_server_id;

                property_entries[0] = { "server_id", server_id };
                participant << (participant_qos << rti::core::policy::Property(
                    property_entries.begin(),
                    property_entries.end(),
                    true
                ));

                application::shutdown_requested = true;
                return;
            }

            if (!server_online)
                std::cout << "Connection to local server has been re-established"
                    << std::endl << std::flush;

            server_online = true;
        } catch ( ... ) {
            if (server_online)
                std::cout << "Local server is no longer accessible;"
                    << " idling until server is back online"
                    << std::endl << std::flush;

            server_online = false;

            rti::util::sleep(dds::core::Duration(1));
            continue;
        }

        if (!may_participate && writer_online) {
            writer.close();
            writer = nullptr;
            writer_online = false;

            std::cout << "Tore down data writer" << std::endl << std::flush;
        } else if (may_participate && !writer_online) {
            writer = dds::pub::DataWriter<State>(publisher, topic);
            writer_online = true;

            std::cout << "Set up data writer" << std::endl << std::flush;
        }

        if (may_participate) {
            State data;
            std::string raw_state;
            std::string commround;
            bool commrounds_differ = true;

            TRY_OR_CONTINUE({
                raw_state = Http::get(server_host, server_port, "state");
                std::string delimiter = ":";

                auto split = raw_state.find(delimiter);
                std::string commround = raw_state.substr(0, split);
                raw_state.erase(0, split + 1);

                commrounds_differ = prev_commround != commround;
                prev_commround = commround;
            });

            if (!may_participate)
                continue;

            if (commrounds_differ) {
                std::cout << "Publishing state for commround " << commround << std::endl;

                data.id(server_id);
                data.state().reserve(raw_state.size());

                std::copy(
                    std::begin(raw_state),
                    std::end(raw_state),
                    std::back_inserter(data.state())
                );

                writer.write(data);
                writer->wait_for_asynchronous_publishing(dds::core::Duration(7200));
            }
        }

        rti::util::sleep(dds::core::Duration(sleep_duration));
    }

#undef TRY_OR_CONTINUE
}

int
main(int argc, char **argv)
{
    using namespace application;

    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit)
        return EXIT_SUCCESS;
    else if (arguments.parse_result == ParseReturn::failure)
        return EXIT_FAILURE;

    setup_signal_handlers();
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_publisher_application(
            arguments.domain_id,
            arguments.server_host,
            arguments.server_port,
            arguments.participation_password
        );
    } catch (std::exception const &ex) {
        std::cerr << "Exception in run_publisher_application(): " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}

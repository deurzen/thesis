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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>
#include <rti/core/policy/CorePolicy.hpp>

#include "participation_snooper.hh"

#include "application.hh"
#include "auth.hh"
#include "common.hh"
#include "http.hh"

#include <stdexcept>
#include <string>
#include <unordered_map>

void
run_subscriber_application(
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

    bool all_proxies_online = false;
    bool proxy_status_tmp = false;
    bool server_online = false;

    std::unordered_map<std::string, RemoteParticipantProxies> remote_participants = {};

    std::string server_id, prev_server_id;
    std::string role = "snooper";

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
        { "entity", "central_server" },
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
    auto participation_snooper = std::make_shared<ParticipationSnooper>(
        participation_password,

        server_id,
        prev_server_id,
        role,
        server_host,
        server_port,

        remote_participants,
        all_proxies_online
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
    participant_reader[0].set_listener(participation_snooper);
    participant.enable();

    std::string next_server_id;
    dds::core::cond::WaitSet waitset;

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

            if (!all_proxies_online && proxy_status_tmp) {
                proxy_status_tmp = all_proxies_online;

                std::cout << "Requesting local server to fallback to offline mode"
                    << std::endl << std::flush;

                Http::post(server_host, server_port, "fallback", "status=on");

            } else if (all_proxies_online && !proxy_status_tmp) {
                proxy_status_tmp = all_proxies_online;

                std::cout << "Requesting local server to go back online"
                    << std::endl << std::flush;

                Http::post(server_host, server_port, "fallback", "status=off");
            }

            if (!server_online) {
                std::cout << "Connection to local server has been re-established"
                    << std::endl << std::flush;

                for (auto &participant_it : remote_participants) {
                    if (participant_it.second.publisher_count > 0)
                        Http::post(
                            server_host,
                            server_port,
                            "participation",
                            "action=add"
                                + ("&id=" + participant_it.first)
                                + "&role=publisher"
                        );

                    if (participant_it.second.subscriber_count > 0)
                        Http::post(
                            server_host,
                            server_port,
                            "participation",
                            "action=add"
                                + ("&id=" + participant_it.first)
                                + "&role=subscriber"
                        );
                }
            }

            server_online = true;
        } catch ( ... ) {
            if (server_online)
                std::cout << "Local server is no longer accessible;"
                    << " idling until server is back online"
                    << std::endl << std::flush;

            server_online = false;
        }

        waitset.dispatch(dds::core::Duration(1));
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
        run_subscriber_application(
            arguments.domain_id,
            arguments.server_host,
            arguments.server_port,
            arguments.participation_password
        );
    } catch (std::exception const &ex) {
        std::cerr << "Exception in run_subscriber_application(): " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}

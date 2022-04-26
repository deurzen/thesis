#ifndef __FL_APP_PARTICIPATION_SNOOPER_HH__
#define __FL_APP_PARTICIPATION_SNOOPER_HH__

#include <dds/domain/discovery.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>

#include "application.hh"
#include "auth.hh"
#include "common.hh"
#include "http.hh"

#include <string>
#include <unordered_map>

class ParticipationSnooper
    : public dds::sub::NoOpDataReaderListener<dds::topic::ParticipantBuiltinTopicData>
{
public:
    ParticipationSnooper(
        std::string const &participation_password,

        std::string const &server_id,
        std::string const &prev_server_id,
        std::string const &role,
        std::string const &server_host,
        std::string const &server_port,

        std::unordered_map<std::string, RemoteParticipantProxies> &remote_participants,
        bool &all_proxies_online
    )
      : dds::sub::NoOpDataReaderListener<dds::topic::ParticipantBuiltinTopicData>(),
        m_participation_password(participation_password),

        m_server_id(server_id),
        m_prev_server_id(server_id),
        m_role(role),
        m_server_host(server_host),
        m_server_port(server_port),

        m_remote_participants(remote_participants),
        m_all_proxies_online(all_proxies_online),
        m_local_proxies({
            .publisher_count = role == "publisher",
            .subscriber_count = role == "subscriber",
            .snooper_count = role == "snooper"
        })
    {}

    void on_data_available(dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData> &reader)
    {
        dds::sub::LoanedSamples<dds::topic::ParticipantBuiltinTopicData> entering_instance_samples
            = reader
                .select()
                .state(dds::sub::status::DataState::new_instance())
                .take();

        dds::sub::LoanedSamples<dds::topic::ParticipantBuiltinTopicData> leaving_instance_samples
            = reader
                .select()
                .state(dds::sub::status::InstanceState::not_alive_mask())
                .take();

        for (auto const &sample : entering_instance_samples) {
            if (!sample.info().valid())
                continue;

            if (!verify_authentication(sample.data(), m_participation_password)) {
                std::cout << "Registration notification with bad password, ignoring participant"
                    << std::endl << std::flush;

                dds::domain::ignore(
                    reader.subscriber().participant(),
                    sample->info().instance_handle()
                );

                continue;
            }

            if (participant_has_server_id(sample.data(), m_server_id)
                || participant_has_server_id(sample.data(), m_prev_server_id))
            {
                std::string participant_role = get_participant_role(sample.data());
                std::cout << "Registration notification from same-server "
                    << participant_role << ", ignoring participant"
                    << std::endl << std::flush;

                bool was_online = m_all_proxies_online;

                if (participant_role == "publisher")
                    ++m_local_proxies.publisher_count;
                else if (participant_role == "subscriber")
                    ++m_local_proxies.subscriber_count;
                else if (participant_role == "snooper")
                    ++m_local_proxies.snooper_count;

                m_all_proxies_online = m_local_proxies.publisher_count > 0
                    && m_local_proxies.subscriber_count > 0
                    && m_local_proxies.snooper_count > 0;

                if (!was_online && m_all_proxies_online)
                    std::cout << "All same-server proxies are now online"
                        << std::endl << std::flush;

                continue;
            }

            std::string participant_server_id = get_participant_server_id(sample.data());
            std::string participant_role = get_participant_role(sample.data());

            std::cout << "Registering " << participant_role
                << " from {" << participant_server_id << "}"
                << std::endl << std::flush;

            auto it = m_remote_participants.find(participant_server_id);
            if (it == m_remote_participants.end())
                m_remote_participants.insert({participant_server_id, {
                    .publisher_count = participant_role == "publisher",
                    .subscriber_count = participant_role == "subscriber"
                }});
            else if (participant_role == "publisher")
                ++(it->second).publisher_count;
            else if (participant_role == "subscriber")
                ++(it->second).subscriber_count;

            while (!application::shutdown_requested)
                try {
                    Http::post(
                        m_server_host,
                        m_server_port,
                        "participation",
                        "action=add"
                            + ("&id=" + participant_server_id)
                            + ("&role=" + participant_role)
                    );

                    break;
                } catch (std::runtime_error const &ex) {
                    std::cerr << ex.what() << std::endl;
                    usleep(TRY_FAILURE_SLEEP_DURATION);
                }
        }

        for (auto const &sample : leaving_instance_samples) {
            if (!sample.info().valid())
                continue;

            if (!verify_authentication(sample.data(), m_participation_password)) {
                std::cout << "Deregistration notification with bad password, ignoring participant"
                    << std::endl << std::flush;

                dds::domain::ignore(
                    reader.subscriber().participant(),
                    sample->info().instance_handle()
                );

                continue;
            }

            if (participant_has_server_id(sample.data(), m_server_id)
                || participant_has_server_id(sample.data(), m_prev_server_id))
            {
                std::string participant_role = get_participant_role(sample.data());
                std::cout << "Deregistration notification from same-server "
                    << participant_role << ", ignoring participant"
                    << std::endl << std::flush;

                if (m_all_proxies_online)
                    std::cout << "Proxy cluster no longer complete;"
                        << " idling until all same-server proxies are online"
                        << std::endl << std::flush;

                if (participant_role == "publisher")
                    --m_local_proxies.publisher_count;
                else if (participant_role == "subscriber")
                    --m_local_proxies.subscriber_count;
                else if (participant_role == "snooper")
                    --m_local_proxies.snooper_count;

                m_all_proxies_online = m_local_proxies.publisher_count > 0
                    && m_local_proxies.subscriber_count > 0
                    && m_local_proxies.snooper_count > 0;

                continue;
            }

            std::string participant_server_id = get_participant_server_id(sample.data());
            std::string participant_role = get_participant_role(sample.data());

            std::cout << "Deregistering " << participant_role
                << " from {" << participant_server_id << "}"
                << std::endl << std::flush;

                auto it = m_remote_participants.find(participant_server_id);

            if (it != m_remote_participants.end()) {
                if (participant_role == "publisher")
                    --(it->second).publisher_count;
                else if (participant_role == "subscriber")
                    --(it->second).subscriber_count;

                if (it->second.publisher_count == 0
                    && it->second.subscriber_count == 0
                ) {
                    m_remote_participants.erase(it);
                }
            }

            while (!application::shutdown_requested)
                try {
                    Http::post(
                        m_server_host,
                        m_server_port,
                        "participation",
                        "action=remove"
                            + ("&id=" + participant_server_id)
                            + ("&role=" + participant_role)
                    );

                    break;
                } catch (std::runtime_error const &ex) {
                    std::cerr << ex.what() << std::endl;
                    usleep(TRY_FAILURE_SLEEP_DURATION);
                }
        }
    }

private:
    std::string const &m_participation_password;

    std::string const &m_server_id;
    std::string const &m_prev_server_id;
    std::string const &m_role;
    std::string const &m_server_host;
    std::string const &m_server_port;

    std::unordered_map<std::string, RemoteParticipantProxies> &m_remote_participants;
    bool &m_all_proxies_online;
    LocalParticipantProxies m_local_proxies;

};

#endif//__FL_APP_PARTICIPATION_SNOOPER_HH__

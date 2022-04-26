#ifndef __FL_APP_AUTH_HH__
#define __FL_APP_AUTH_HH__

#include <dds/domain/discovery.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>
#include <rti/core/policy/CorePolicy.hpp>

#include "common.hh"

#include <string>

struct ParticipantProxies {
    short publisher_count;
    short subscriber_count;
};

bool
verify_authentication(
    dds::topic::ParticipantBuiltinTopicData const &participant_data,
    std::string const &participation_password
)
{
    dds::core::ByteSeq const &user_data = participant_data.user_data().value();
    std::string received_password(user_data.begin(), user_data.end());

    return received_password == participation_password;
}

class AuthenticationListener
  : public dds::sub::NoOpDataReaderListener<dds::topic::ParticipantBuiltinTopicData>
{
public:
    AuthenticationListener(
        std::string const &participation_password,

        std::string const &server_id,
        std::string const &prev_server_id,
        std::string const &role,

        bool &all_proxies_online
    )
      : dds::sub::NoOpDataReaderListener<dds::topic::ParticipantBuiltinTopicData>(),
        m_participation_password(participation_password),

        m_server_id(server_id),
        m_prev_server_id(server_id),
        m_role(role),

        m_all_proxies_online(all_proxies_online),
        m_local_proxies({
            .publisher_count = role == "publisher",
            .subscriber_count = role == "subscriber",
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
                    << participant_role
                    << std::endl << std::flush;

                bool was_online = m_all_proxies_online;

                if (participant_role == "publisher")
                    ++m_local_proxies.publisher_count;
                else if (participant_role == "subscriber")
                    ++m_local_proxies.subscriber_count;

                m_all_proxies_online = m_local_proxies.publisher_count > 0
                    && m_local_proxies.subscriber_count > 0;

                if (!was_online && m_all_proxies_online)
                    std::cout << "All same-server proxies are now online"
                        << std::endl << std::flush;

                continue;
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
                    << participant_role
                    << std::endl << std::flush;

                if (m_all_proxies_online)
                    std::cout << "Proxy cluster no longer complete;"
                        << " idling until all same-server proxies are online"
                        << std::endl << std::flush;

                if (participant_role == "publisher")
                    --m_local_proxies.publisher_count;
                else if (participant_role == "subscriber")
                    --m_local_proxies.subscriber_count;

                m_all_proxies_online = m_local_proxies.publisher_count > 0
                    && m_local_proxies.subscriber_count > 0;

                continue;
            }
        }
    }

private:
    std::string const &m_participation_password;

    std::string const &m_server_id;
    std::string const &m_prev_server_id;
    std::string const &m_role;

    bool &m_all_proxies_online;
    ParticipantProxies m_local_proxies;

};

#endif//__FL_APP_AUTH_HH__

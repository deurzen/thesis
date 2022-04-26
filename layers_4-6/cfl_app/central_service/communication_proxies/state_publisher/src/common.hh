#ifndef __FL_APP_COMMON_HH__
#define __FL_APP_COMMON_HH__

#include <string>

#include <dds/domain/discovery.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>
#include <rti/core/policy/CorePolicy.hpp>

constexpr unsigned TRY_FAILURE_SLEEP_DURATION = 500000;

bool
participant_has_server_id(
    dds::topic::ParticipantBuiltinTopicData const& participant_data,
    std::string const& server_id
)
{
    return std::string(participant_data->property().get("server_id")) == server_id;
}


std::string
get_participant_server_id(
    dds::topic::ParticipantBuiltinTopicData const& participant_data
)
{
    return std::string(participant_data->property().get("server_id"));
}

std::string
get_participant_role(
    dds::topic::ParticipantBuiltinTopicData const& participant_data
)
{
    return std::string(participant_data->property().get("role"));
}

#endif//__FL_APP_COMMON_HH__

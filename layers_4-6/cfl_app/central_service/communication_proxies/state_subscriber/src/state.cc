

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from state.idl
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#include <iosfwd>
#include <iomanip>

#include "rti/topic/cdr/Serialization.hpp"

#include "state.hh"
#include "state_plugin.hh"

#include <rti/util/ostream_operators.hpp>

// ---- State: 

State::State() :
    m_id_ ("")  {
}   

State::State (
    const std::string& id,
    const ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& state)
    :
        m_id_( id ),
        m_state_( state ) {
}

#ifdef RTI_CXX11_RVALUE_REFERENCES
#ifdef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
State::State(State&& other_) OMG_NOEXCEPT  :m_id_ (std::move(other_.m_id_))
,
m_state_ (std::move(other_.m_state_))
{
} 

State& State::operator=(State&&  other_) OMG_NOEXCEPT {
    State tmp(std::move(other_));
    swap(tmp); 
    return *this;
}
#endif
#endif   

void State::swap(State& other_)  OMG_NOEXCEPT 
{
    using std::swap;
    swap(m_id_, other_.m_id_);
    swap(m_state_, other_.m_state_);
}  

bool State::operator == (const State& other_) const {
    if (m_id_ != other_.m_id_) {
        return false;
    }
    if (m_state_ != other_.m_state_) {
        return false;
    }
    return true;
}
bool State::operator != (const State& other_) const {
    return !this->operator ==(other_);
}

std::ostream& operator << (std::ostream& o,const State& sample)
{
    ::rti::util::StreamFlagSaver flag_saver (o);
    o <<"[";
    o << "id: " << sample.id()<<", ";
    o << "state: " << sample.state() ;
    o <<"]";
    return o;
}

// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        #ifndef NDDS_STANDALONE_TYPE
        template<>
        struct native_type_code< State > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode State_g_tc_id_string;
                static DDS_TypeCode State_g_tc_state_sequence;

                static DDS_TypeCode_Member State_g_tc_members[2]=
                {

                    {
                        (char *)"id",/* Member name */
                        {
                            0,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_KEY_MEMBER , /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"state",/* Member name */
                        {
                            1,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }
                };

                static DDS_TypeCode State_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"State", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        2, /* Number of members */
                        State_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for State*/

                if (is_initialized) {
                    return &State_g_tc;
                }

                State_g_tc_id_string = initialize_string_typecode((10L));
                State_g_tc_state_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) > >(((MAX_SEQUENCE_LENGTH)));

                State_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                State_g_tc_state_sequence._data._typeCode = (RTICdrTypeCode *)&DDS_g_tc_char;
                State_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&State_g_tc_id_string;
                State_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)& State_g_tc_state_sequence;

                /* Initialize the values for member annotations. */
                State_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_STRING;
                State_g_tc_members[0]._annotations._defaultValue._u.string_value = (DDS_Char *) "";

                State_g_tc._data._sampleAccessInfo = sample_access_info();
                State_g_tc._data._typePlugin = type_plugin_info();    

                is_initialized = RTI_TRUE;

                return &State_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                State *sample;

                static RTIXCdrMemberAccessInfo State_g_memberAccessInfos[2] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo State_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &State_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    State);
                if (sample == NULL) {
                    return NULL;
                }

                State_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->id() - (char *)sample);

                State_g_memberAccessInfos[1].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->state() - (char *)sample);

                State_g_sampleAccessInfo.memberAccessInfos = 
                State_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(State);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        State_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        State_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                State_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                State_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< State >;

                State_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &State_g_sampleAccessInfo;
            }

            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin State_g_typePlugin = 
                {
                    NULL, /* serialize */
                    NULL, /* serialize_key */
                    NULL, /* deserialize_sample */
                    NULL, /* deserialize_key_sample */
                    NULL, /* skip */
                    NULL, /* get_serialized_sample_size */
                    NULL, /* get_serialized_sample_max_size_ex */
                    NULL, /* get_serialized_key_max_size_ex */
                    NULL, /* get_serialized_sample_min_size */
                    NULL, /* serialized_sample_to_key */
                    NULL,
                    NULL,
                    NULL,
                    NULL
                };

                return &State_g_typePlugin;
            }
        }; // native_type_code
        #endif

        const ::dds::core::xtypes::StructType& dynamic_type< State >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< State >::get())));
        }

    }
}

namespace dds { 
    namespace topic {
        void topic_type_support< State >:: register_type(
            ::dds::domain::DomainParticipant& participant,
            const std::string& type_name) 
        {

            ::rti::domain::register_type_plugin(
                participant,
                type_name,
                StatePlugin_new,
                StatePlugin_delete);
        }

        std::vector<char>& topic_type_support< State >::to_cdr_buffer(
            std::vector<char>& buffer, 
            const State& sample,
            ::dds::core::policy::DataRepresentationId representation)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = StatePlugin_serialize_to_cdr_buffer(
                NULL, 
                &length,
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = StatePlugin_serialize_to_cdr_buffer(
                &buffer[0], 
                &length, 
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;
        }

        void topic_type_support< State >::from_cdr_buffer(State& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = StatePlugin_deserialize_from_cdr_buffer(
                &sample, 
                &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            ::rti::core::check_return_code(ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
            "Failed to create State from cdr buffer");
        }

        void topic_type_support< State >::reset_sample(State& sample) 
        {
            sample.id("");
            ::rti::topic::reset_sample(sample.state());
        }

        void topic_type_support< State >::allocate_sample(State& sample, int, int) 
        {
            ::rti::topic::allocate_sample(sample.id(),  -1, 10L);
            ::rti::topic::allocate_sample(sample.state(),  (MAX_SEQUENCE_LENGTH), -1);
        }

    }
}  




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

// ---- GlobalState: 

GlobalState::GlobalState() :
    m_commround_ ("")  {
}   

GlobalState::GlobalState (
    const std::string& commround,
    const ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& state)
    :
        m_commround_( commround ),
        m_state_( state ) {
}

#ifdef RTI_CXX11_RVALUE_REFERENCES
#ifdef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
GlobalState::GlobalState(GlobalState&& other_) OMG_NOEXCEPT  :m_commround_ (std::move(other_.m_commround_))
,
m_state_ (std::move(other_.m_state_))
{
} 

GlobalState& GlobalState::operator=(GlobalState&&  other_) OMG_NOEXCEPT {
    GlobalState tmp(std::move(other_));
    swap(tmp); 
    return *this;
}
#endif
#endif   

void GlobalState::swap(GlobalState& other_)  OMG_NOEXCEPT 
{
    using std::swap;
    swap(m_commround_, other_.m_commround_);
    swap(m_state_, other_.m_state_);
}  

bool GlobalState::operator == (const GlobalState& other_) const {
    if (m_commround_ != other_.m_commround_) {
        return false;
    }
    if (m_state_ != other_.m_state_) {
        return false;
    }
    return true;
}
bool GlobalState::operator != (const GlobalState& other_) const {
    return !this->operator ==(other_);
}

std::ostream& operator << (std::ostream& o,const GlobalState& sample)
{
    ::rti::util::StreamFlagSaver flag_saver (o);
    o <<"[";
    o << "commround: " << sample.commround()<<", ";
    o << "state: " << sample.state() ;
    o <<"]";
    return o;
}

// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        #ifndef NDDS_STANDALONE_TYPE
        template<>
        struct native_type_code< GlobalState > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode GlobalState_g_tc_commround_string;
                static DDS_TypeCode GlobalState_g_tc_state_sequence;

                static DDS_TypeCode_Member GlobalState_g_tc_members[2]=
                {

                    {
                        (char *)"commround",/* Member name */
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
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
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

                static DDS_TypeCode GlobalState_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"GlobalState", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        2, /* Number of members */
                        GlobalState_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for GlobalState*/

                if (is_initialized) {
                    return &GlobalState_g_tc;
                }

                GlobalState_g_tc_commround_string = initialize_string_typecode((255L));
                GlobalState_g_tc_state_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) > >(((MAX_SEQUENCE_LENGTH)));

                GlobalState_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                GlobalState_g_tc_state_sequence._data._typeCode = (RTICdrTypeCode *)&DDS_g_tc_char;
                GlobalState_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&GlobalState_g_tc_commround_string;
                GlobalState_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)& GlobalState_g_tc_state_sequence;

                /* Initialize the values for member annotations. */
                GlobalState_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_STRING;
                GlobalState_g_tc_members[0]._annotations._defaultValue._u.string_value = (DDS_Char *) "";

                GlobalState_g_tc._data._sampleAccessInfo = sample_access_info();
                GlobalState_g_tc._data._typePlugin = type_plugin_info();    

                is_initialized = RTI_TRUE;

                return &GlobalState_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                GlobalState *sample;

                static RTIXCdrMemberAccessInfo GlobalState_g_memberAccessInfos[2] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo GlobalState_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &GlobalState_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    GlobalState);
                if (sample == NULL) {
                    return NULL;
                }

                GlobalState_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->commround() - (char *)sample);

                GlobalState_g_memberAccessInfos[1].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->state() - (char *)sample);

                GlobalState_g_sampleAccessInfo.memberAccessInfos = 
                GlobalState_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(GlobalState);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        GlobalState_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        GlobalState_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                GlobalState_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                GlobalState_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< GlobalState >;

                GlobalState_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &GlobalState_g_sampleAccessInfo;
            }

            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin GlobalState_g_typePlugin = 
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

                return &GlobalState_g_typePlugin;
            }
        }; // native_type_code
        #endif

        const ::dds::core::xtypes::StructType& dynamic_type< GlobalState >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< GlobalState >::get())));
        }

    }
}

namespace dds { 
    namespace topic {
        void topic_type_support< GlobalState >:: register_type(
            ::dds::domain::DomainParticipant& participant,
            const std::string& type_name) 
        {

            ::rti::domain::register_type_plugin(
                participant,
                type_name,
                GlobalStatePlugin_new,
                GlobalStatePlugin_delete);
        }

        std::vector<char>& topic_type_support< GlobalState >::to_cdr_buffer(
            std::vector<char>& buffer, 
            const GlobalState& sample,
            ::dds::core::policy::DataRepresentationId representation)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = GlobalStatePlugin_serialize_to_cdr_buffer(
                NULL, 
                &length,
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = GlobalStatePlugin_serialize_to_cdr_buffer(
                &buffer[0], 
                &length, 
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;
        }

        void topic_type_support< GlobalState >::from_cdr_buffer(GlobalState& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = GlobalStatePlugin_deserialize_from_cdr_buffer(
                &sample, 
                &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            ::rti::core::check_return_code(ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
            "Failed to create GlobalState from cdr buffer");
        }

        void topic_type_support< GlobalState >::reset_sample(GlobalState& sample) 
        {
            sample.commround("");
            ::rti::topic::reset_sample(sample.state());
        }

        void topic_type_support< GlobalState >::allocate_sample(GlobalState& sample, int, int) 
        {
            ::rti::topic::allocate_sample(sample.commround(),  -1, 255L);
            ::rti::topic::allocate_sample(sample.state(),  (MAX_SEQUENCE_LENGTH), -1);
        }

    }
}  


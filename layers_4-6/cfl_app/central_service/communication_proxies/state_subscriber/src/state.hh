

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from state.idl
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#ifndef state_2087021748_hpp
#define state_2087021748_hpp

#include <iosfwd>

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport __declspec(dllexport)
#endif

#include "dds/domain/DomainParticipant.hpp"
#include "dds/topic/TopicTraits.hpp"
#include "dds/core/SafeEnumeration.hpp"
#include "dds/core/String.hpp"
#include "dds/core/array.hpp"
#include "dds/core/vector.hpp"
#include "dds/core/Optional.hpp"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/StructType.hpp"
#include "dds/core/xtypes/UnionType.hpp"
#include "dds/core/xtypes/EnumType.hpp"
#include "dds/core/xtypes/AliasType.hpp"
#include "rti/core/array.hpp"
#include "rti/core/BoundedSequence.hpp"
#include "rti/util/StreamFlagSaver.hpp"
#include "rti/domain/PluginSupport.hpp"
#include "rti/core/LongDouble.hpp"
#include "dds/core/External.hpp"
#include "rti/core/Pointer.hpp"
#include "rti/topic/TopicTraits.hpp"

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

static const int32_t MAX_SEQUENCE_LENGTH = 100000000L;

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
// On Windows, dll-export template instantiations of standard types used by
// other dll-exported types
template class NDDSUSERDllExport std::allocator< char >;
template class NDDSUSERDllExport std::vector< char >;
#endif
class NDDSUSERDllExport State {
  public:
    State();

    State(
        const std::string& id,
        const ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& state);

    #ifdef RTI_CXX11_RVALUE_REFERENCES
    #ifndef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
    State (State&&) = default;
    State& operator=(State&&) = default;
    State& operator=(const State&) = default;
    State(const State&) = default;
    #else
    State(State&& other_) OMG_NOEXCEPT;
    State& operator=(State&&  other_) OMG_NOEXCEPT;
    #endif
    #endif

    std::string& id() OMG_NOEXCEPT {
        return m_id_;
    }

    const std::string& id() const OMG_NOEXCEPT {
        return m_id_;
    }

    void id(const std::string& value) {
        m_id_ = value;
    }

    void id(std::string&& value) {
        m_id_ = std::move(value);
    }
    ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& state() OMG_NOEXCEPT {
        return m_state_;
    }

    const ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& state() const OMG_NOEXCEPT {
        return m_state_;
    }

    void state(const ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >& value) {
        m_state_ = value;
    }

    void state(::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) >&& value) {
        m_state_ = std::move(value);
    }

    bool operator == (const State& other_) const;
    bool operator != (const State& other_) const;

    void swap(State& other_) OMG_NOEXCEPT ;

  private:

    std::string m_id_;
    ::rti::core::bounded_sequence< char, (MAX_SEQUENCE_LENGTH) > m_state_;

};

inline void swap(State& a, State& b)  OMG_NOEXCEPT
{
    a.swap(b);
}

NDDSUSERDllExport std::ostream& operator<<(std::ostream& o, const State& sample);

namespace rti {
    namespace flat {
        namespace topic {
        }
    }
}
namespace dds {
    namespace topic {

        template<>
        struct topic_type_name< State > {
            NDDSUSERDllExport static std::string value() {
                return "State";
            }
        };

        template<>
        struct is_topic_type< State > : public ::dds::core::true_type {};

        template<>
        struct topic_type_support< State > {
            NDDSUSERDllExport
            static void register_type(
                ::dds::domain::DomainParticipant& participant,
                const std::string & type_name);

            NDDSUSERDllExport
            static std::vector<char>& to_cdr_buffer(
                std::vector<char>& buffer,
                const State& sample,
                ::dds::core::policy::DataRepresentationId representation
                = ::dds::core::policy::DataRepresentation::auto_id());

            NDDSUSERDllExport
            static void from_cdr_buffer(State& sample, const std::vector<char>& buffer);
            NDDSUSERDllExport
            static void reset_sample(State& sample);

            NDDSUSERDllExport
            static void allocate_sample(State& sample, int, int);

            static const ::rti::topic::TypePluginKind::type type_plugin_kind =
            ::rti::topic::TypePluginKind::STL;
        };

    }
}

namespace rti {
    namespace topic {

        #ifndef NDDS_STANDALONE_TYPE
        template<>
        struct dynamic_type< State > {
            typedef ::dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const ::dds::core::xtypes::StructType& get();
        };
        #endif

        template <>
        struct extensibility< State > {
            static const ::dds::core::xtypes::ExtensibilityKind::type kind =
            ::dds::core::xtypes::ExtensibilityKind::EXTENSIBLE;
        };

    }
}

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif // state_2087021748_hpp


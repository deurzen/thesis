

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from state.idl
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#ifndef statePlugin_2087021748_h
#define statePlugin_2087021748_h

#include "state.hh"

struct RTICdrStream;

#ifndef pres_typePlugin_h
#include "pres/pres_typePlugin.h"
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

/* The type used to store keys for instances of type struct
* AnotherSimple.
*
* By default, this type is struct State
* itself. However, if for some reason this choice is not practical for your
* system (e.g. if sizeof(struct State)
* is very large), you may redefine this typedef in terms of another type of
* your choosing. HOWEVER, if you define the KeyHolder type to be something
* other than struct AnotherSimple, the
* following restriction applies: the key of struct
* State must consist of a
* single field of your redefined KeyHolder type and that field must be the
* first field in struct State.
*/
typedef  class State StateKeyHolder;

#define StatePlugin_get_sample PRESTypePluginDefaultEndpointData_getSample

#define StatePlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
#define StatePlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer

#define StatePlugin_get_key PRESTypePluginDefaultEndpointData_getKey 
#define StatePlugin_return_key PRESTypePluginDefaultEndpointData_returnKey

#define StatePlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
#define StatePlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

/* --------------------------------------------------------------------------------------
Support functions:
* -------------------------------------------------------------------------------------- */

NDDSUSERDllExport extern State*
StatePluginSupport_create_data_w_params(
    const struct DDS_TypeAllocationParams_t * alloc_params);

NDDSUSERDllExport extern State*
StatePluginSupport_create_data_ex(RTIBool allocate_pointers);

NDDSUSERDllExport extern State*
StatePluginSupport_create_data(void);

NDDSUSERDllExport extern RTIBool 
StatePluginSupport_copy_data(
    State *out,
    const State *in);

NDDSUSERDllExport extern void 
StatePluginSupport_destroy_data_w_params(
    State *sample,
    const struct DDS_TypeDeallocationParams_t * dealloc_params);

NDDSUSERDllExport extern void 
StatePluginSupport_destroy_data_ex(
    State *sample,RTIBool deallocate_pointers);

NDDSUSERDllExport extern void 
StatePluginSupport_destroy_data(
    State *sample);

NDDSUSERDllExport extern void 
StatePluginSupport_print_data(
    const State *sample,
    const char *desc,
    unsigned int indent);

NDDSUSERDllExport extern State*
StatePluginSupport_create_key_ex(RTIBool allocate_pointers);

NDDSUSERDllExport extern State*
StatePluginSupport_create_key(void);

NDDSUSERDllExport extern void 
StatePluginSupport_destroy_key_ex(
    StateKeyHolder *key,RTIBool deallocate_pointers);

NDDSUSERDllExport extern void 
StatePluginSupport_destroy_key(
    StateKeyHolder *key);

/* ----------------------------------------------------------------------------
Callback functions:
* ---------------------------------------------------------------------------- */

NDDSUSERDllExport extern PRESTypePluginParticipantData 
StatePlugin_on_participant_attached(
    void *registration_data, 
    const struct PRESTypePluginParticipantInfo *participant_info,
    RTIBool top_level_registration, 
    void *container_plugin_context,
    RTICdrTypeCode *typeCode);

NDDSUSERDllExport extern void 
StatePlugin_on_participant_detached(
    PRESTypePluginParticipantData participant_data);

NDDSUSERDllExport extern PRESTypePluginEndpointData 
StatePlugin_on_endpoint_attached(
    PRESTypePluginParticipantData participant_data,
    const struct PRESTypePluginEndpointInfo *endpoint_info,
    RTIBool top_level_registration, 
    void *container_plugin_context);

NDDSUSERDllExport extern void 
StatePlugin_on_endpoint_detached(
    PRESTypePluginEndpointData endpoint_data);

NDDSUSERDllExport extern void    
StatePlugin_return_sample(
    PRESTypePluginEndpointData endpoint_data,
    State *sample,
    void *handle);    

NDDSUSERDllExport extern RTIBool 
StatePlugin_copy_sample(
    PRESTypePluginEndpointData endpoint_data,
    State *out,
    const State *in);

/* ----------------------------------------------------------------------------
(De)Serialize functions:
* ------------------------------------------------------------------------- */

NDDSUSERDllExport extern RTIBool
StatePlugin_serialize_to_cdr_buffer(
    char * buffer,
    unsigned int * length,
    const State *sample,
    ::dds::core::policy::DataRepresentationId representation
    = ::dds::core::policy::DataRepresentation::xcdr()); 

NDDSUSERDllExport extern RTIBool 
StatePlugin_deserialize(
    PRESTypePluginEndpointData endpoint_data,
    State **sample, 
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool
StatePlugin_deserialize_from_cdr_buffer(
    State *sample,
    const char * buffer,
    unsigned int length);    

NDDSUSERDllExport extern unsigned int 
StatePlugin_get_serialized_sample_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

/* --------------------------------------------------------------------------------------
Key Management functions:
* -------------------------------------------------------------------------------------- */
NDDSUSERDllExport extern PRESTypePluginKeyKind 
StatePlugin_get_key_kind(void);

NDDSUSERDllExport extern unsigned int 
StatePlugin_get_serialized_key_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern unsigned int 
StatePlugin_get_serialized_key_max_size_for_keyhash(
    PRESTypePluginEndpointData endpoint_data,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern RTIBool 
StatePlugin_deserialize_key(
    PRESTypePluginEndpointData endpoint_data,
    State ** sample,
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_key,
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool 
StatePlugin_instance_to_key(
    PRESTypePluginEndpointData endpoint_data,
    StateKeyHolder *key, 
    const State *instance);

NDDSUSERDllExport extern RTIBool 
StatePlugin_key_to_instance(
    PRESTypePluginEndpointData endpoint_data,
    State *instance, 
    const StateKeyHolder *key);

NDDSUSERDllExport extern RTIBool 
StatePlugin_serialized_sample_to_keyhash(
    PRESTypePluginEndpointData endpoint_data,
    struct RTICdrStream *stream, 
    DDS_KeyHash_t *keyhash,
    RTIBool deserialize_encapsulation,
    void *endpoint_plugin_qos); 

/* Plugin Functions */
NDDSUSERDllExport extern struct PRESTypePlugin*
StatePlugin_new(void);

NDDSUSERDllExport extern void
StatePlugin_delete(struct PRESTypePlugin *);

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif /* statePlugin_2087021748_h */


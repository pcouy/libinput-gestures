#ifndef CONFIG_H
#define CONFIG_H
#include "libinput-gestures.h"

//#define DEBUG

#include "libcyaml/include/cyaml/cyaml.h"

/* Schema for string pointer values (used in sequences of strings). */
static const cyaml_schema_value_t string_ptr_schema = {
    CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};

// Enum mappings to yaml string values
static const cyaml_strval_t gesture_type_strings[] = {
    { "swipe", SWIPE },
    { "pinch", PINCH },
};

static const cyaml_strval_t swipe_direction_strings[] = {
    { "left", LEFT },
    { "right", RIGHT },
    { "up", UP },
    { "down", DOWN },
    { "none", NONE },
};

static const cyaml_strval_t trigger_type_strings[] = {
    { "end", ON_END },
    { "threshold", ON_THRESHOLD },
    { "repeat", REPEAT },
};

// Schema for parsing triggers
static const cyaml_schema_field_t trigger_fields_schema[] = {
    CYAML_FIELD_ENUM("type", CYAML_FLAG_DEFAULT, struct trigger, gesture, gesture_type_strings, CYAML_ARRAY_LEN(gesture_type_strings)),
    CYAML_FIELD_ENUM("direction", CYAML_FLAG_OPTIONAL, struct trigger, swipe_direction, swipe_direction_strings, CYAML_ARRAY_LEN(swipe_direction_strings)),
    CYAML_FIELD_UINT("fingers", CYAML_FLAG_DEFAULT, struct trigger, fingers),
    CYAML_FIELD_ENUM("trigger_on", CYAML_FLAG_DEFAULT, struct trigger, type, trigger_type_strings, CYAML_ARRAY_LEN(trigger_type_strings)),
    CYAML_FIELD_STRING_PTR("config", CYAML_FLAG_POINTER, struct trigger, config_name, 0, CYAML_UNLIMITED),
    CYAML_FIELD_SEQUENCE("command", CYAML_FLAG_POINTER, struct trigger, cmd.args, &string_ptr_schema, 0, CYAML_UNLIMITED),
    CYAML_FIELD_END
};

static const cyaml_schema_value_t config_triggers_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct trigger, trigger_fields_schema),
};

// Schema and helper struct for parsing trigger configs
struct named_trigger_config {
    char *name;
    struct trigger_config conf;
};

static const cyaml_schema_field_t config_fields_schema[] = {
    CYAML_FIELD_FLOAT("threshold", CYAML_FLAG_OPTIONAL, struct trigger_config, threshold),
    CYAML_FIELD_UINT("min_duration", CYAML_FLAG_OPTIONAL, struct trigger_config, min_duration),
    CYAML_FIELD_UINT("max_duration", CYAML_FLAG_OPTIONAL, struct trigger_config, max_duration),
    CYAML_FIELD_END
};

static const cyaml_schema_field_t trigger_config_fields_schema[] = {
    CYAML_FIELD_STRING_PTR("name", CYAML_FLAG_POINTER, struct named_trigger_config, name, 0, CYAML_UNLIMITED),
    CYAML_FIELD_MAPPING("config", CYAML_FLAG_OPTIONAL, struct named_trigger_config, conf, config_fields_schema),
    CYAML_FIELD_END
};

static const cyaml_schema_value_t config_trigger_configs_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct named_trigger_config, trigger_config_fields_schema),
};

// Schema and helper struct for parsing whole config file
struct full_config_t {
    struct trigger *triggers;
    unsigned triggers_count;

    struct named_trigger_config *trigger_configs;
    unsigned trigger_configs_count;
};

static const cyaml_schema_field_t triggers_schema[] = {
    CYAML_FIELD_SEQUENCE("triggers", CYAML_FLAG_POINTER, struct full_config_t, triggers, &config_triggers_schema, 0, CYAML_UNLIMITED),
    CYAML_FIELD_SEQUENCE("trigger_configs", CYAML_FLAG_POINTER, struct full_config_t, trigger_configs, &config_trigger_configs_schema, 0, CYAML_UNLIMITED),
    CYAML_FIELD_END
};

static const cyaml_schema_value_t all_triggers_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct full_config_t, triggers_schema),
};

struct full_config_t* load_yaml();
#endif

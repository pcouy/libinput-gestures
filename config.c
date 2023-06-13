#include <stdio.h>
#include <string.h>

#include "config.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include "libinput-gestures.h"

struct full_config_t* load_yaml()
{
    struct full_config_t *all_triggers;
    const cyaml_config_t cyaml_conf = {
        .log_fn = cyaml_log,
        .mem_fn = cyaml_mem,
        .log_level = CYAML_LOG_WARNING,
    };
    cyaml_err_t err = cyaml_load_file("config.yaml", &cyaml_conf, &all_triggers_schema, (void **) &all_triggers, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "ERROR: %s\n", cyaml_strerror(err));
        return NULL;
    }

    // Match trigger configs based on name and update trigger instances
    for (int i_trigger=0; i_trigger<all_triggers->triggers_count; i_trigger++) {
        struct trigger trig = all_triggers->triggers[i_trigger];
        int name_found = 0;
        for (int i_config=0; i_config<all_triggers->trigger_configs_count; i_config++) {
            char *conf_name = all_triggers->trigger_configs[i_config].name;
            struct trigger_config conf = all_triggers->trigger_configs[i_config].conf;
            if(strcmp(trig.config_name, conf_name) == 0) {
                all_triggers->triggers[i_trigger].config.threshold = conf.threshold;
                all_triggers->triggers[i_trigger].config.min_duration = conf.min_duration;
                all_triggers->triggers[i_trigger].config.max_duration = conf.max_duration;
                name_found = 1;
                break;
            }
        }
        if (!name_found) {
            printf("ERROR : No trigger config named '%s' : using default of 0 for all values\n", trig.config_name);
        }
    }

    return all_triggers;
}

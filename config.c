#include <stdio.h>
#include <string.h>

#include "config.h"
#include "libcyaml/include/cyaml/cyaml.h"
#include "libinput-gestures.h"

struct full_config_t* find_and_load_yaml(int argc, char* argv[])
{
    struct full_config_t *all_triggers;
    if (argc >= 2) {
        all_triggers = load_yaml(argv[1]);
    } else {
        all_triggers = NULL;
    }

    if (all_triggers == NULL) {
        all_triggers = load_yaml("config.yaml");
    }
    if (all_triggers == NULL) {
        char *home = getenv("HOME");
        if (home != NULL) {
            int home_len = strlen(home);
            char *inside_path = "/.config/libinput-gestures.yaml";
            int inside_len = strlen(inside_path);
            if (home_len + inside_len < 250) {
                char user_path[255];
                snprintf(user_path, 255, "%s%s", home, inside_path);
                all_triggers = load_yaml(user_path);
            }
        }
    }
    if (all_triggers == NULL) {
        all_triggers = load_yaml("/etc/libinput-gestures.yaml");
    }
    if (all_triggers == NULL) {
        all_triggers = malloc(sizeof(struct full_config_t));
        all_triggers->triggers_count = 0;
        all_triggers->trigger_configs_count = 0;
        all_triggers->triggers = NULL;
        all_triggers->trigger_configs = NULL;
    }
    return all_triggers;
}

struct full_config_t* load_yaml(char* yaml_file)
{
    struct full_config_t *all_triggers;
    const cyaml_config_t cyaml_conf = {
        .log_fn = cyaml_log,
        .mem_fn = cyaml_mem,
        .log_level = CYAML_LOG_WARNING,
    };
    printf("Reading config from '%s'\n", yaml_file);
    cyaml_err_t err = cyaml_load_file(yaml_file, &cyaml_conf, &all_triggers_schema, (void **) &all_triggers, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "ERROR (reading '%s') : %s\n", yaml_file, cyaml_strerror(err));
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
            printf("WARNING: No trigger config named '%s' : using default of 0 for all values\n", trig.config_name);
        }
    }

    return all_triggers;
}

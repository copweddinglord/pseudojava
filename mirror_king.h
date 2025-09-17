/**
 * MIRROR King - Adaptive Whitelist Generation System
 *
 * Extends Junk Warrior with a self-generating whitelist through mirror rule
 * transmutation. Converts identified junk patterns into whitelist entries
 * through reflection principles.
 */

#ifndef MIRROR_KING_H
#define MIRROR_KING_H

#include "junk_warrior.h"

/* Constants */
#define MIRROR_MAX_WHITELIST 100        /* Maximum whitelist entries */
#define MIRROR_REFLECTION_THRESHOLD 0.7  /* Minimum certainty for reflection */
#define MIRROR_STABILITY_FACTOR 1.1      /* Stabilization factor for whitelist entries */
#define MIRROR_PROMOTION_COUNT 3         /* Required occurrences before promotion */

/* Data structures */

/* Whitelist entry - mirrored from junk */
typedef struct {
    FuzzyValue pattern;                 /* The accepted pattern */
    double confidence;                  /* Confidence in this whitelist entry */
    int occurrence_count;               /* Number of times this pattern was observed */
    time_t creation_time;               /* When this entry was first created */
    time_t last_match_time;             /* When this entry was last matched */
} WhitelistEntry;

/* Mirror system */
typedef struct {
    WhitelistEntry whitelist[MIRROR_MAX_WHITELIST];
    size_t whitelist_count;
    double reflection_threshold;
    double stability_factor;
    int promotion_threshold;
    int auto_promote;                   /* Whether to automatically promote patterns */
} MirrorKing;

/* Global instance */
static MirrorKing mirror_king;

/* Function declarations */
void init_mirror_king(void);
int check_whitelist(const FuzzyValue* fuzz);
void reflect_junk_to_whitelist(void);
void promote_junk_item(const JunkItem* item);
double calculate_reflection_value(const JunkItem* item);
int are_patterns_similar(const FuzzyValue* a, const FuzzyValue* b);
void stabilize_whitelist(void);
void print_whitelist_status(void);
void mirror_process(void);

/* Macros */
#define MIRROR_KING_INIT() init_mirror_king()
#define MIRROR_CHECK(fuzz) check_whitelist(fuzz)
#define MIRROR_REFLECT() reflect_junk_to_whitelist()
#define MIRROR_PROCESS() mirror_process()
#define MIRROR_STATUS() print_whitelist_status()

/* Function implementations */

/* Initialize the mirror king system */
void init_mirror_king(void) {
    /* Initialize Junk Warrior if not already done */
    if (junk_warrior.health == 0.0) {
        JUNK_WARRIOR_INIT();
    }
    
    mirror_king.whitelist_count = 0;
    mirror_king.reflection_threshold = MIRROR_REFLECTION_THRESHOLD;
    mirror_king.stability_factor = MIRROR_STABILITY_FACTOR;
    mirror_king.promotion_threshold = MIRROR_PROMOTION_COUNT;
    mirror_king.auto_promote = 1;
    
    for (size_t i = 0; i < MIRROR_MAX_WHITELIST; i++) {
        mirror_king.whitelist[i].pattern.value[0] = 0.0;
        mirror_king.whitelist[i].pattern.value[1] = 0.0;
        mirror_king.whitelist[i].pattern.certainty = 0.0;
        mirror_king.whitelist[i].pattern.label = NULL;
        mirror_king.whitelist[i].confidence = 0.0;
        mirror_king.whitelist[i].occurrence_count = 0;
        mirror_king.whitelist[i].creation_time = 0;
        mirror_king.whitelist[i].last_match_time = 0;
    }
}

/* Check if a fuzzy value matches any whitelist entry */
int check_whitelist(const FuzzyValue* fuzz) {
    if (fuzz == NULL) return 0;
    
    for (size_t i = 0; i < mirror_king.whitelist_count; i++) {
        if (are_patterns_similar(fuzz, &mirror_king.whitelist[i].pattern)) {
            /* Update last match time and occurrence count */
            mirror_king.whitelist[i].last_match_time = time(NULL);
            mirror_king.whitelist[i].occurrence_count++;
            
            /* Gradually increase confidence with each match */
            mirror_king.whitelist[i].confidence = 
                fmin(1.0, mirror_king.whitelist[i].confidence + 0.05);
                
            return 1; /* Found in whitelist */
        }
    }
    
    return 0; /* Not in whitelist */
}

/* Check if two patterns are similar enough to be considered the same */
int are_patterns_similar(const FuzzyValue* a, const FuzzyValue* b) {
    if (a == NULL || b == NULL) return 0;
    
    /* Calculate Euclidean distance in 2D space */
    double dx = a->value[0] - b->value[0];
    double dy = a->value[1] - b->value[1];
    double distance = sqrt(dx*dx + dy*dy);
    
    /* Calculate certainty similarity */
    double cert_diff = fabs(a->certainty - b->certainty);
    
    /* Patterns are similar if they're close in both space and certainty */
    return (distance < 0.2) && (cert_diff < 0.3);
}

/* Calculate reflection value for a junk item */
double calculate_reflection_value(const JunkItem* item) {
    if (item == NULL) return 0.0;
    
    /* Base reflection value on certainty and priority */
    double base_value = item->fuzz.certainty * (1.0 - (item->priority / 10.0));
    
    /* Adjust based on processing status */
    if (item->processed) {
        base_value *= 1.2; /* Boost processed items */
    }
    
    /* Age adjustment - newer items get slight preference */
    time_t now = time(NULL);
    double age_factor = 1.0 - fmin(1.0, (now - item->timestamp) / (24.0 * 3600.0));
    
    return base_value * (0.8 + 0.2 * age_factor);
}

/* Promote a junk item to the whitelist through reflection */
void promote_junk_item(const JunkItem* item) {
    if (item == NULL || mirror_king.whitelist_count >= MIRROR_MAX_WHITELIST) return;
    
    /* Check if this pattern is already similar to something in the whitelist */
    for (size_t i = 0; i < mirror_king.whitelist_count; i++) {
        if (are_patterns_similar(&item->fuzz, &mirror_king.whitelist[i].pattern)) {
            /* Already exists, just update confidence */
            mirror_king.whitelist[i].confidence = 
                fmin(1.0, mirror_king.whitelist[i].confidence + 0.1);
            mirror_king.whitelist[i].occurrence_count++;
            mirror_king.whitelist[i].last_match_time = time(NULL);
            return;
        }
    }
    
    /* Create new whitelist entry */
    size_t index = mirror_king.whitelist_count++;
    
    /* Mirror the junk item with transformation */
    mirror_king.whitelist[index].pattern.value[0] = item->fuzz.value[0] * mirror_king.stability_factor;
    mirror_king.whitelist[index].pattern.value[1] = item->fuzz.value[1] * mirror_king.stability_factor;
    mirror_king.whitelist[index].pattern.certainty = 
        fmin(1.0, item->fuzz.certainty * mirror_king.stability_factor);
    
    /* Copy or create label */
    if (item->fuzz.label) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "mirror_%s", item->fuzz.label);
        mirror_king.whitelist[index].pattern.label = strdup(buffer);
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "mirror_item_%zu", index);
        mirror_king.whitelist[index].pattern.label = strdup(buffer);
    }
    
    /* Set metadata */
    mirror_king.whitelist[index].confidence = 0.6; /* Start with moderate confidence */
    mirror_king.whitelist[index].occurrence_count = 1;
    mirror_king.whitelist[index].creation_time = time(NULL);
    mirror_king.whitelist[index].last_match_time = time(NULL);
    
    printf("MIRROR King: Promoted junk item to whitelist: %s\n", 
           mirror_king.whitelist[index].pattern.label);
}

/* Reflect junk items to whitelist based on patterns */
void reflect_junk_to_whitelist(void) {
    /* First pass: calculate reflection values for all junk items */
    double reflection_values[JUNK_WARRIOR_MAX_JUNK] = {0.0};
    size_t candidates[JUNK_WARRIOR_MAX_JUNK] = {0};
    size_t candidate_count = 0;
    
    for (size_t i = 0; i < junk_warrior.junk_count; i++) {
        reflection_values[i] = calculate_reflection_value(&junk_warrior.junk_items[i]);
        
        /* If reflection value exceeds threshold, mark as candidate */
        if (reflection_values[i] >= mirror_king.reflection_threshold) {
            candidates[candidate_count++] = i;
        }
    }
    
    /* Second pass: promote top candidates (up to 3) */
    size_t promotions = 0;
    size_t max_promotions = 3; /* Limit promotions per cycle */
    
    while (promotions < max_promotions && candidate_count > 0) {
        /* Find candidate with highest reflection value */
        double max_value = -1.0;
        size_t max_index = 0;
        size_t max_position = 0;
        
        for (size_t i = 0; i < candidate_count; i++) {
            size_t junk_index = candidates[i];
            if (reflection_values[junk_index] > max_value) {
                max_value = reflection_values[junk_index];
                max_index = junk_index;
                max_position = i;
            }
        }
        
        /* Promote the best candidate */
        promote_junk_item(&junk_warrior.junk_items[max_index]);
        promotions++;
        
        /* Remove this candidate from further consideration */
        if (max_position < candidate_count - 1) {
            memmove(&candidates[max_position], &candidates[max_position + 1], 
                    (candidate_count - max_position - 1) * sizeof(size_t));
        }
        candidate_count--;
    }
    
    if (promotions > 0) {
        printf("MIRROR King: Promoted %zu junk items to whitelist\n", promotions);
    }
}

/* Stabilize whitelist by removing stale entries and adjusting confidence */
void stabilize_whitelist(void) {
    time_t now = time(NULL);
    size_t i = 0;
    
    while (i < mirror_king.whitelist_count) {
        /* Check if entry is stale (unused for 7 days) */
        if ((now - mirror_king.whitelist[i].last_match_time) > 7 * 24 * 3600) {
            /* Remove this entry by replacing it with the last one */
            if (mirror_king.whitelist[i].pattern.label) {
                free(mirror_king.whitelist[i].pattern.label);
            }
            
            if (i < mirror_king.whitelist_count - 1) {
                mirror_king.whitelist[i] = mirror_king.whitelist[mirror_king.whitelist_count - 1];
            }
            
            mirror_king.whitelist_count--;
            printf("MIRROR King: Removed stale whitelist entry\n");
            
            /* Don't increment i since we need to check the entry we just moved */
        } else {
            /* Gradually decay confidence for entries that haven't been matched recently */
            double days_since_match = (now - mirror_king.whitelist[i].last_match_time) / (24.0 * 3600.0);
            if (days_since_match > 1.0) {
                mirror_king.whitelist[i].confidence *= (1.0 - 0.01 * days_since_match);
                
                /* If confidence drops too low, remove the entry */
                if (mirror_king.whitelist[i].confidence < 0.2) {
                    if (mirror_king.whitelist[i].pattern.label) {
                        free(mirror_king.whitelist[i].pattern.label);
                    }
                    
                    if (i < mirror_king.whitelist_count - 1) {
                        mirror_king.whitelist[i] = mirror_king.whitelist[mirror_king.whitelist_count - 1];
                    }
                    
                    mirror_king.whitelist_count--;
                    printf("MIRROR King: Removed low-confidence whitelist entry\n");
                    
                    /* Don't increment i */
                    continue;
                }
            }
            
            i++; /* Move to next entry */
        }
    }
}

/* Print whitelist status */
void print_whitelist_status(void) {
    printf("=== MIRROR King Whitelist Status ===\n");
    printf("Whitelist Entries: %zu/%d\n", mirror_king.whitelist_count, MIRROR_MAX_WHITELIST);
    printf("Reflection Threshold: %.2f\n", mirror_king.reflection_threshold);
    printf("Auto-Promotion: %s\n", mirror_king.auto_promote ? "ON" : "OFF");
    
    if (mirror_king.whitelist_count > 0) {
        printf("\nWhitelist Entries:\n");
        size_t show_count = (mirror_king.whitelist_count < 5) ? mirror_king.whitelist_count : 5;
        for (size_t i = 0; i < show_count; i++) {
            const char* label = mirror_king.whitelist[i].pattern.label ? 
                               mirror_king.whitelist[i].pattern.label : "Unnamed";
            printf("  [%zu] %s: (%.2f, %.2f) [Conf: %.2f, Occur: %d]\n",
                   i, label,
                   mirror_king.whitelist[i].pattern.value[0],
                   mirror_king.whitelist[i].pattern.value[1],
                   mirror_king.whitelist[i].confidence,
                   mirror_king.whitelist[i].occurrence_count);
        }
    }
}

/* Main processing function */
void mirror_process(void) {
    /* First process junk conservatively */
    JUNK_WARRIOR_PROCESS();
    
    /* Then reflect junk to whitelist if auto-promote is enabled */
    if (mirror_king.auto_promote) {
        reflect_junk_to_whitelist();
    }
    
    /* Periodically stabilize the whitelist (every ~10 calls) */
    static int call_count = 0;
    if (++call_count % 10 == 0) {
        stabilize_whitelist();
    }
}

#endif /* MIRROR_KING_H */
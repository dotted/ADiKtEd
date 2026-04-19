/******************************************************************************/
/** @file lev_script.c
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Functions for maintaining the level script.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     24 Jan 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include <string.h>

#include "lev_script.h"

#include "globals.h"
#include "arr_utils.h"
#include "lev_data.h"
#include "obj_column_def.h"
#include "obj_slabs.h"
#include "obj_column.h"
#include "lev_column.h"
#include "graffiti.h"
#include "lev_things.h"
#include "obj_actnpts.h"
#include "msg_log.h"

/* Conditional statements */
const char if_cmdtext[]="IF";
const char if_available_cmdtext[]="IF_AVAILABLE";
const char if_actnpt_cmdtext[]="IF_ACTION_POINT";
const char endif_cmdtext[]="ENDIF";
/* CMD_CONDIT */
const char *cmd_condit_arr[]={
        "",if_cmdtext,if_available_cmdtext,  /*0x000,0x001,0x002 */
        if_actnpt_cmdtext,endif_cmdtext,     /*0x003,0x004 */
        };

/* ADiKtEd-specific commands */
const char custom_column_cmdtext[]="CUSTOM_COLUMN";
const char graffiti_cmdtext[]="GRAFFITI";
const char leveltimestmp_cmdtext[]="LEVEL_TIMESTAMP";
const char usrcmnds_count_cmdtext[]="USER_COMMANDS_COUNT";
const char levversion_cmdtext[]="LEVEL_VERSION";
const char levname_cmdtext[]="LEVEL_NAME";
const char levdesc_cmdtext[]="LEVEL_DESCRIPTION";
const char levauthors_cmdtext[]="LEVEL_AUTHORS";

/* CMD_ADIKTED - adikted specific group */
const char *cmd_adikted_arr[]={
        "",custom_column_cmdtext,graffiti_cmdtext,  /*0x000,0x001,0x002 */
        leveltimestmp_cmdtext,usrcmnds_count_cmdtext,levversion_cmdtext,
        levname_cmdtext, levdesc_cmdtext, levauthors_cmdtext,
        };

/* Partys - commands */
const char create_party_cmdtext[]="CREATE_PARTY";
const char add_tunneler_tolev_cmdtext[]="ADD_TUNNELLER_TO_LEVEL";
const char add_to_party_cmdtext[]="ADD_TO_PARTY";
const char add_party_tolev_cmdtext[]="ADD_PARTY_TO_LEVEL";
const char add_creatr_tolev_cmdtext[]="ADD_CREATURE_TO_LEVEL";
const char add_tunneller_party_tolev_cmdtext[]="ADD_TUNNELLER_PARTY_TO_LEVEL";
/* CMD_PARTY */
const char *cmd_party_arr[]={
        "",create_party_cmdtext,add_tunneler_tolev_cmdtext,  /*0x000,0x001,0x002 */
        add_to_party_cmdtext,add_party_tolev_cmdtext,add_creatr_tolev_cmdtext,/*0x003,0x004,0x005 */
        add_tunneller_party_tolev_cmdtext,/*0x006 */
        };

/* Availability of various items */
const char room_avail_cmdtext[]="ROOM_AVAILABLE";
const char creatr_avail_cmdtext[]="CREATURE_AVAILABLE";
const char magic_avail_cmdtext[]="MAGIC_AVAILABLE";
const char trap_avail_cmdtext[]="TRAP_AVAILABLE";
const char door_avail_cmdtext[]="DOOR_AVAILABLE";
/* CMD_AVAIL */
const char *cmd_avail_arr[]={
        "",room_avail_cmdtext,creatr_avail_cmdtext,  /*0x000,0x001,0x002 */
        magic_avail_cmdtext,trap_avail_cmdtext,door_avail_cmdtext,
        };

/*Custom objectives */
const char display_objectv_cmdtext[]="DISPLAY_OBJECTIVE";
const char display_objectv_wpos_cmdtext[]="DISPLAY_OBJECTIVE_WITH_POS";
const char display_info_cmdtext[]="DISPLAY_INFORMATION";
const char display_info_wpos_cmdtext[]="DISPLAY_INFORMATION_WITH_POS";
const char bonus_lev_time_cmdtext[]="BONUS_LEVEL_TIME";
const char tutorial_flash_btn_cmdtext[]="TUTORIAL_FLASH_BUTTON";
const char quick_objectv_cmdtext[]="QUICK_OBJECTIVE";
const char quick_info_cmdtext[]="QUICK_INFORMATION";
/* CMD_CUSTOBJ - objectives and info boxes */
const char *cmd_custobj_arr[]={
        "",display_objectv_cmdtext,display_objectv_wpos_cmdtext,
        display_info_cmdtext,display_info_wpos_cmdtext,bonus_lev_time_cmdtext,
        tutorial_flash_btn_cmdtext,quick_objectv_cmdtext,quick_info_cmdtext,
        };

/*Setup commands */
const char set_gen_speed_cmdtext[]="SET_GENERATE_SPEED";
const char start_money_cmdtext[]="START_MONEY";
const char comp_player_cmdtext[]="COMPUTER_PLAYER";
const char ally_players_cmdtext[]="ALLY_PLAYERS";
const char set_hate_cmdtext[]="SET_HATE";
const char resrch_cmdtext[]="RESEARCH";
const char set_computer_globals_cmdtext[]="SET_COMPUTER_GLOBALS";
const char set_computer_checks_cmdtext[]="SET_COMPUTER_CHECKS";
const char set_computer_event_cmdtext[]="SET_COMPUTER_EVENT";
const char set_computer_process_cmdtext[]="SET_COMPUTER_PROCESS";
const char max_creatures_cmdtext[]="MAX_CREATURES";
/* CMD_SETUP - setup commands */
const char *cmd_setup_arr[]={
        "",set_gen_speed_cmdtext,start_money_cmdtext,
        resrch_cmdtext,comp_player_cmdtext,ally_players_cmdtext,
        set_hate_cmdtext,
        set_computer_globals_cmdtext,set_computer_checks_cmdtext,
        set_computer_event_cmdtext,set_computer_process_cmdtext,
        max_creatures_cmdtext,};

/*Action/Trigger commands */
const char reset_actnpt_cmdtext[]="RESET_ACTION_POINT";
const char next_cmd_reusable_cmdtext[]="NEXT_COMMAND_REUSABLE";
const char set_flag_cmdtext[]="SET_FLAG";
const char set_timer_cmdtext[]="SET_TIMER";
const char game_lose_cmdtext[]="LOSE_GAME";
const char game_win_cmdtext[]="WIN_GAME";
const char set_music_cmdtext[]="SET_MUSIC";
/* CMD_TRIGER - Action/Trigger commands */
const char *cmd_triger_arr[]={
        "",reset_actnpt_cmdtext,next_cmd_reusable_cmdtext,
        set_flag_cmdtext,set_timer_cmdtext,
        game_lose_cmdtext,game_win_cmdtext,
        set_music_cmdtext,};

/*Creatures adjusting */
const char dead_creatures_ret_to_pool_cmdtext[]="DEAD_CREATURES_RETURN_TO_POOL";
const char add_creatr_to_pool_cmdtext[]="ADD_CREATURE_TO_POOL";
const char set_creatr_fear_cmdtext[]="SET_CREATURE_FEAR";
const char set_creatr_armour_cmdtext[]="SET_CREATURE_ARMOUR";
const char set_creatr_health_cmdtext[]="SET_CREATURE_HEALTH";
const char set_creatr_strength_cmdtext[]="SET_CREATURE_STRENGTH";
const char set_creatr_max_level_cmdtext[]="SET_CREATURE_MAX_LEVEL";
const char creatr_swap_cmdtext[]="SWAP_CREATURE";
/* CMD_CRTRADJ - objectives and info boxes */
const char *cmd_crtradj_arr[]={
        "",dead_creatures_ret_to_pool_cmdtext,
        add_creatr_to_pool_cmdtext,set_creatr_max_level_cmdtext,
        set_creatr_strength_cmdtext,set_creatr_health_cmdtext,
        set_creatr_armour_cmdtext,set_creatr_fear_cmdtext,
        creatr_swap_cmdtext,
        };

/* Comments/skipped commands */
const char rem_cmdtext[]="REM";
const char emptyln_cmdtext[]="";
const char *cmd_commnt_arr[]={
        "",emptyln_cmdtext,rem_cmdtext,
        };

/* Obsolete commands */
const char print_cmdtext[]="PRINT";
const char create_text_cmdtext[]="CREATE_TEXT";
const char *cmd_obsolt_arr[]={
        "",print_cmdtext,create_text_cmdtext,
        };

/* Special commands - replacing numbers */
const char random_cmdtext[]="RANDOM";


/*Variables - Flags */
const char flag0_cmdtext[]="FLAG0";
const char flag1_cmdtext[]="FLAG1";
const char flag2_cmdtext[]="FLAG2";
const char flag3_cmdtext[]="FLAG3";
const char flag4_cmdtext[]="FLAG4";
const char flag5_cmdtext[]="FLAG5";
const char flag6_cmdtext[]="FLAG6";
const char flag7_cmdtext[]="FLAG7";
/* CMD_FLAG - flags */
const char *cmd_flag_arr[]={
        "",flag0_cmdtext,flag1_cmdtext,  /*0x000,0x001,0x002 */
        flag2_cmdtext,flag3_cmdtext,flag4_cmdtext,
        flag5_cmdtext,flag6_cmdtext,flag7_cmdtext,
        };

/*Variables - Timers */
const char timer0_cmdtext[]="TIMER0";
const char timer1_cmdtext[]="TIMER1";
const char timer2_cmdtext[]="TIMER2";
const char timer3_cmdtext[]="TIMER3";
const char timer4_cmdtext[]="TIMER4";
const char timer5_cmdtext[]="TIMER5";
const char timer6_cmdtext[]="TIMER6";
const char timer7_cmdtext[]="TIMER7";
/* CMD_TIMER - timers */
const char *cmd_timer_arr[]={
        "",timer0_cmdtext,timer1_cmdtext,  /*0x000,0x001,0x002 */
        timer2_cmdtext,timer3_cmdtext,timer4_cmdtext,
        timer5_cmdtext,timer6_cmdtext,timer7_cmdtext,
        };

/*Operators */
const char oper_equ_cmdtext[]="==";
const char oper_neq_cmdtext[]="!=";
const char oper_less_cmdtext[]="<";
const char oper_grtr_cmdtext[]=">";
const char oper_lesseq_cmdtext[]="<=";
const char oper_grtreq_cmdtext[]=">=";
/* CMD_OPERATR - arithmetic and logic operators */
const char *cmd_operator_arr[]={
        "",oper_equ_cmdtext,oper_neq_cmdtext,  /*0x000,0x001,0x002 */
        oper_less_cmdtext,oper_grtr_cmdtext,oper_lesseq_cmdtext,
        oper_grtreq_cmdtext,
        };

/*Variables */
const char all_dngns_destroyed_cmdtext[]="ALL_DUNGEONS_DESTROYED";
const char crtrs_scavng_gained_cmdtext[]="CREATURES_SCAVENGED_GAINED";
const char crtrs_scavng_lost_cmdtext[]="CREATURES_SCAVENGED_LOST";
const char doors_destroyed_cmdtext[]="DOORS_DESTROYED";
const char total_gold_mined_cmdtext[]="TOTAL_GOLD_MINED";
const char dngn_destroyed_cmdtext[]="DUNGEON_DESTROYED";
const char gold_pots_stolen_cmdtext[]="GOLD_POTS_STOLEN";
const char times_broken_into_cmdtext[]="TIMES_BROKEN_INTO";
const char spells_stolen_cmdtext[]="SPELLS_STOLEN";
const char rooms_destroyed_cmdtext[]="ROOMS_DESTROYED";
const char battles_won_cmdtext[]="BATTLES_WON";
const char battles_lost_cmdtext[]="BATTLES_LOST";
const char creatrs_annoyed_cmdtext[]="CREATURES_ANNOYED";
const char total_creatrs_left_cmdtext[]="TOTAL_CREATURES_LEFT";
const char total_area_cmdtext[]="TOTAL_AREA";
const char total_doors_cmdtext[]="TOTAL_DOORS";
const char total_research_cmdtext[]="TOTAL_RESEARCH";
const char total_creatrs_cmdtext[]="TOTAL_CREATURES";
const char total_imps_cmdtext[]="TOTAL_IMPS";
const char break_in_cmdtext[]="BREAK_IN";
const char game_turn_cmdtext[]="GAME_TURN";
const char money_cmdtext[]="MONEY";
/* CMD_VARIBL - variables used in conditional statements */
const char *cmd_variabl_arr[]={
        "",all_dngns_destroyed_cmdtext,crtrs_scavng_gained_cmdtext,  /*0x000,0x001,0x002 */
        crtrs_scavng_lost_cmdtext,doors_destroyed_cmdtext,total_gold_mined_cmdtext,
        dngn_destroyed_cmdtext,gold_pots_stolen_cmdtext,times_broken_into_cmdtext,
        spells_stolen_cmdtext,rooms_destroyed_cmdtext,battles_won_cmdtext,
        battles_lost_cmdtext,creatrs_annoyed_cmdtext,total_creatrs_left_cmdtext,
        total_area_cmdtext,total_doors_cmdtext,total_research_cmdtext,
        total_creatrs_cmdtext,total_imps_cmdtext,break_in_cmdtext,
        game_turn_cmdtext,money_cmdtext,
        };

/*Objectives of creatures in partys */
const char objctv_defend_party_cmdtext[]="DEFEND_PARTY";
const char objctv_attack_rooms_cmdtext[]="ATTACK_ROOMS";
const char objctv_attack_dnheart_cmdtext[]="ATTACK_DUNGEON_HEART";
const char objctv_enemies_cmdtext[]="ATTACK_ENEMIES";
const char objctv_steal_spells_cmdtext[]="STEAL_SPELLS";
const char objctv_steal_gold_cmdtext[]="STEAL_GOLD";
/*Heading objectives for tunneller partys */
const char objctv_approp_dung_cmdtext[]="APPROPIATE_DUNGEON";
const char objctv_dnheart_cmdtext[]="DUNGEON_HEART";
const char objctv_dungeon_cmdtext[]="DUNGEON";
const char objctv_actnpt_cmdtext[]="ACTION_POINT";
/* CMD_OBJCTV - objectives of creatures in partys */
const char *cmd_party_objectv_arr[]={
        "",objctv_actnpt_cmdtext,objctv_dungeon_cmdtext,  /*0x000,0x001,0x002 */
        objctv_dnheart_cmdtext,objctv_approp_dung_cmdtext,
        objctv_steal_gold_cmdtext,objctv_steal_spells_cmdtext,objctv_enemies_cmdtext,
        objctv_attack_dnheart_cmdtext,objctv_attack_rooms_cmdtext,objctv_defend_party_cmdtext,
        };

/*Computer player tweaks (undocumented) */
const char comp_sell_trapsdoors_cmdtext[]="COMPUTER_SELL_TRAPS_AND_DOORS";
const char comp_attack_magic_cmdtext[]="COMPUTER_ATTACK_MAGIC";
const char comp_wait_for_bridge_cmdtext[]="COMPUTER_WAIT_FOR_BRIDGE";
const char comp_magic_speed_up_cmdtext[]="COMPUTER_MAGIC_SPEED_UP";
const char comp_dig_to_neutral_cmdtext[]="COMPUTER_DIG_TO_NEUTRAL";
const char comp_slap_imps_cmdtext[]="COMPUTER_SLAP_IMPS";
const char comp_move_creatrs_todefens_cmdtext[]="COMPUTER_MOVE_CREATURES_TO_DEFEND";
const char comp_move_creatrs_topos_cmdtext[]="COMPUTER_MOVE_CREATURE_TO_POS";
const char comp_move_creatrs_toroom_cmdtext[]="COMPUTER_MOVE_CREATURE_TO_ROOM";
const char comp_pickup_for_attack_cmdtext[]="COMPUTER_PICKUP_FOR_ATTACK";
const char comp_magic_callarms_cmdtext[]="COMPUTER_MAGIC_CALL_TO_ARMS";
const char comp_dig_toattack_cmdtext[]="COMPUTER_DIG_TO_ATTACK";
const char comp_dig_togold_cmdtext[]="COMPUTER_DIG_TO_GOLD";
const char comp_dig_toentrnce_cmdtext[]="COMPUTER_DIG_TO_ENTRANCE";
const char comp_place_room_cmdtext[]="COMPUTER_PLACE_ROOM";
const char comp_check_room_dug_cmdtext[]="COMPUTER_CHECK_ROOM_DUG";
const char comp_dig_room_cmdtext[]="COMPUTER_DIG_ROOM";
const char comp_dig_room_passage_cmdtext[]="COMPUTER_DIG_ROOM_PASSAGE";
/* CMD_COMP - computer player tweaks */
const char *cmd_comp_plyr_arr[]={
        "",comp_sell_trapsdoors_cmdtext,comp_attack_magic_cmdtext,  /*0x000,0x001,0x002 */
        comp_wait_for_bridge_cmdtext,comp_magic_speed_up_cmdtext,comp_dig_to_neutral_cmdtext,
        comp_slap_imps_cmdtext,comp_move_creatrs_todefens_cmdtext,comp_move_creatrs_topos_cmdtext,
        comp_move_creatrs_toroom_cmdtext,comp_pickup_for_attack_cmdtext,comp_magic_callarms_cmdtext,
        comp_dig_toattack_cmdtext,comp_dig_togold_cmdtext,comp_dig_toentrnce_cmdtext,
        comp_place_room_cmdtext,comp_check_room_dug_cmdtext,comp_dig_room_cmdtext,
        comp_dig_room_passage_cmdtext,
        };

const char obj_room_cmdtext[]="ROOM";
const char obj_magic_cmdtext[]="MAGIC"; /*Note: magic is also type of door */
const char obj_creature_cmdtext[]="CREATURE";
/* CMD_OBJTYPE */
/* note that \t is only used to make the string nonempty */
const char *cmd_objtype_arr[]={
        "",obj_creature_cmdtext,"\t",    /*0x000,0x001,0x002 */
        obj_room_cmdtext,obj_magic_cmdtext,
        };

/*Workshop - Doors */
const char door_steel_cmdtext[]="STEEL";
const char door_braced_cmdtext[]="BRACED";
const char door_wood_cmdtext[]="WOOD";
const char door_magic_cmdtext[]="MAGIC"; /*Note: magic is also type of object */
/* CMD_DOOR */
const char *cmd_doors_arr[]={
        "", door_wood_cmdtext, door_braced_cmdtext,  /*0x000,0x001,0x002 */
        door_steel_cmdtext,door_magic_cmdtext,
        };
const char *cmd_doors_fullname[]={
        "", "Wooden", "Braced",  /*0x000,0x001,0x002 */
        "Iron", "Magic",
        };

/*Workshop - Traps */
const char trap_boulder_cmdtext[]="BOULDER";
const char trap_alarm_cmdtext[]="ALARM";
const char trap_gas_cmdtext[]="POISON_GAS";
const char trap_lightng_cmdtext[]="LIGHTNING";
const char trap_wordpwr_cmdtext[]="WORD_OF_POWER";
const char trap_lava_cmdtext[]="LAVA";
/* CMD_TRAP */
const char *cmd_traps_arr[]={
        "", trap_boulder_cmdtext, trap_alarm_cmdtext,  /*0x000,0x001,0x002 */
        trap_gas_cmdtext,trap_lightng_cmdtext,trap_wordpwr_cmdtext,
        trap_lava_cmdtext,
        };
const char *cmd_traps_fullname[]={
        "", "Boulder", "Alarm",  /*0x000,0x001,0x002 */
        "Poison gas", "Lightning", "Word of Power",
        "Lava",
        };

/*Spells */
const char spell_armag_cmdtext[]="POWER_ARMAGEDDON";
const char spell_possess_cmdtext[]="POWER_POSSESS";
const char spell_destrwalls_cmdtext[]="POWER_DESTROY_WALLS";
const char spell_chickn_cmdtext[]="POWER_CHICKEN";
const char spell_disease_cmdtext[]="POWER_DISEASE";
const char spell_conceal_cmdtext[]="POWER_CONCEAL";
const char spell_protect_cmdtext[]="POWER_PROTECT";
const char spell_speed_cmdtext[]="POWER_SPEED";
const char spell_lightng_cmdtext[]="POWER_LIGHTNING";
const char spell_holdaud_cmdtext[]="POWER_HOLD_AUDIENCE";
const char spell_healcrtr_cmdtext[]="POWER_HEAL_CREATURE";
const char spell_cavein_cmdtext[]="POWER_CAVE_IN";
const char spell_callarms_cmdtext[]="POWER_CALL_TO_ARMS";
const char spell_sight_cmdtext[]="POWER_SIGHT";
const char spell_slap_cmdtext[]="POWER_SLAP";
const char spell_obey_cmdtext[]="POWER_OBEY";
const char spell_imp_cmdtext[]="POWER_IMP";
const char spell_hand_cmdtext[]="POWER_HAND";
/* CMD_SPELL */
const char *cmd_spells_arr[]={
        "", spell_hand_cmdtext, spell_imp_cmdtext,  /*0x000,0x001,0x002 */
        spell_obey_cmdtext,spell_slap_cmdtext,spell_sight_cmdtext,
        spell_callarms_cmdtext,spell_cavein_cmdtext,spell_healcrtr_cmdtext,
        spell_holdaud_cmdtext,spell_lightng_cmdtext,spell_speed_cmdtext,
        spell_protect_cmdtext,spell_conceal_cmdtext,spell_disease_cmdtext,
        spell_chickn_cmdtext,spell_destrwalls_cmdtext,spell_possess_cmdtext,
        spell_armag_cmdtext,
        };
const char *cmd_spells_fullname[]={
        "", "Hand of Evil", "Create imp",  /*0x000,0x001,0x002 */
        "Must obey", "Slap", "Evil Sight",
        "Call to arms", "Cave in", "Heal creature",
        "Hold audience", "Lightning", "Speed",
        "Protect", "Conceal", "Disease",
        "Chicken", "Destroy walls", "Possess",
        "Armageddon",
        };

/*Players */
const char player_all_cmdtext[]="ALL_PLAYERS";
const char player_good_cmdtext[]="PLAYER_GOOD";
const char player3_cmdtext[]="PLAYER3";
const char player2_cmdtext[]="PLAYER2";
const char player1_cmdtext[]="PLAYER1";
const char player0_cmdtext[]="PLAYER0";
/* CMD_PLAYER - target player selection */
const char *cmd_players_arr[]={
        player0_cmdtext,player1_cmdtext,player2_cmdtext,  /*0x000,0x001,0x002 */
        player3_cmdtext,player_good_cmdtext,"\t",  /*0x003,0x004,0x005 */
        player_all_cmdtext,
        };
/*Evil Creatures */
const char creatr_horny_cmdtext[]="HORNY";
const char creatr_skeleton_cmdtext[]="SKELETON";
const char creatr_troll_cmdtext[]="TROLL";
const char creatr_dragon_cmdtext[]="DRAGON";
const char creatr_dmspawn_cmdtext[]="DEMONSPAWN";
const char creatr_fly_cmdtext[]="FLY";
const char creatr_dkmist_cmdtext[]="DARK_MISTRESS";
const char creatr_sorcer_cmdtext[]="SORCEROR";
const char creatr_biledm_cmdtext[]="BILE_DEMON";
const char creatr_imp_cmdtext[]="IMP";
const char creatr_bug_cmdtext[]="BUG";
const char creatr_vampire_cmdtext[]="VAMPIRE";
const char creatr_spider_cmdtext[]="SPIDER";
const char creatr_hound_cmdtext[]="HELL_HOUND";
const char creatr_ghost_cmdtext[]="GHOST";
const char creatr_tentacl_cmdtext[]="TENTACLE";
const char creatr_orc_cmdtext[]="ORC";
/*Good Creatures */
const char creatr_wizard_cmdtext[]="WIZARD";
const char creatr_barbarin_cmdtext[]="BARBARIAN";
const char creatr_archr_cmdtext[]="ARCHER";
const char creatr_monk_cmdtext[]="MONK";
const char creatr_dwarfa_cmdtext[]="DWARFA";
const char creatr_knight_cmdtext[]="KNIGHT";
const char creatr_avatar_cmdtext[]="AVATAR";
const char creatr_tunnelr_cmdtext[]="TUNNELLER";
const char creatr_witch_cmdtext[]="WITCH";
const char creatr_giant_cmdtext[]="GIANT";
const char creatr_fairy_cmdtext[]="FAIRY";
const char creatr_thief_cmdtext[]="THIEF";
const char creatr_samurai_cmdtext[]="SAMURAI";
const char creatr_newa_cmdtext[]="NEW_CREATURE_B";
const char creatr_newb_cmdtext[]="NEW_CREATURE_A";
const char creatr_flspirit_cmdtext[]="FLOATING_SPIRIT";

/* CMD_CREATR - creature type selection */
const char *cmd_creatures_arr[]={
        "", creatr_wizard_cmdtext, creatr_barbarin_cmdtext,  /*0x000,0x001,0x002 */
        creatr_archr_cmdtext, creatr_monk_cmdtext, creatr_dwarfa_cmdtext,
        creatr_knight_cmdtext, creatr_avatar_cmdtext, creatr_tunnelr_cmdtext,
        creatr_witch_cmdtext, creatr_giant_cmdtext, creatr_fairy_cmdtext,
        creatr_thief_cmdtext, creatr_samurai_cmdtext, creatr_horny_cmdtext,
        creatr_skeleton_cmdtext, creatr_troll_cmdtext, creatr_dragon_cmdtext,
        creatr_dmspawn_cmdtext, creatr_fly_cmdtext, creatr_dkmist_cmdtext,
        creatr_sorcer_cmdtext, creatr_biledm_cmdtext, creatr_imp_cmdtext,
        creatr_bug_cmdtext, creatr_vampire_cmdtext, creatr_spider_cmdtext,
        creatr_hound_cmdtext, creatr_ghost_cmdtext, creatr_tentacl_cmdtext,
        creatr_orc_cmdtext,  creatr_flspirit_cmdtext, creatr_newa_cmdtext,
        creatr_newb_cmdtext,
        };
/*Rooms */
const char room_grdpost_cmdtext[]="GUARD_POST";
const char room_brige_cmdtext[]="BRIDGE";
const char room_lair_cmdtext[]="LAIR";
const char room_hatchery_cmdtext[]="GARDEN";
const char room_barracks_cmdtext[]="BARRACKS";
const char room_graveyrd_cmdtext[]="GRAVEYARD";
const char room_temple_cmdtext[]="TEMPLE";
const char room_scavng_cmdtext[]="SCAVENGER";
const char room_workshp_cmdtext[]="WORKSHOP";
const char room_training_cmdtext[]="TRAINING";
const char room_torture_cmdtext[]="TORTURE";
const char room_prison_cmdtext[]="PRISON";
/*Note: resrch_cmdtext is same, so when reading */
/*  the value RESEARCH should be set instead of the one from room list */
const char room_library_cmdtext[]="RESEARCH";
const char room_treasure_cmdtext[]="TREASURE";
const char room_entrance_cmdtext[]="ENTRANCE";
/* CMD_ROOM */
const char *cmd_rooms_arr[]={
        "", room_entrance_cmdtext, room_treasure_cmdtext,  /*0x000,0x001,0x002 */
        room_library_cmdtext,room_prison_cmdtext,room_torture_cmdtext,
        room_training_cmdtext,room_workshp_cmdtext,room_scavng_cmdtext,
        room_temple_cmdtext,room_graveyrd_cmdtext,room_hatchery_cmdtext,
        room_lair_cmdtext,room_barracks_cmdtext,room_brige_cmdtext,
        room_grdpost_cmdtext,
        };

/* Orientations */
const char orient_ns_cmdtext[]="ORIENT_NS";
const char orient_we_cmdtext[]="ORIENT_WE";
const char orient_sn_cmdtext[]="ORIENT_SN";
const char orient_ew_cmdtext[]="ORIENT_EW";
const char orient_tpns_cmdtext[]="ORIENT_TOPNS";
const char orient_tpwe_cmdtext[]="ORIENT_TOPWE";
const char orient_tpsn_cmdtext[]="ORIENT_TOPSN";
const char orient_tpew_cmdtext[]="ORIENT_TOPEW";

const unsigned short orient_constants[]={
        ORIENT_NS,ORIENT_WE,ORIENT_SN,ORIENT_EW,
        ORIENT_TNS,ORIENT_TWE,ORIENT_TSN,ORIENT_TEW
        };
/* CMD_ORIENT - adikted specific group */
const char *cmd_orient_arr[]={
        orient_ns_cmdtext,orient_we_cmdtext,orient_sn_cmdtext,      /*0x000,0x001,0x002 */
        orient_ew_cmdtext,orient_tpns_cmdtext, orient_tpwe_cmdtext, /*0x003,0x004,0x005 */
        orient_tpsn_cmdtext,orient_tpew_cmdtext,                    /*0x006,0x007 */
        };

/* Graffiti fonts */
const char font_none_cmdtext[]="FONT_NONE";
const char font_adiclssc_cmdtext[]="FONT_ADICLSSC";
const char font_adisize8_cmdtext[]="FONT_ADISIZE8";

const char *cmd_font_arr[]={
        font_none_cmdtext,font_adiclssc_cmdtext,font_adisize8_cmdtext,   /*0x000,0x001,0x002 */
        };

const char *cmd_orient_shortnames[]={
        "NS","WE","SN","EW","TpNS","TpWE","TpSN","TpEW" };

const char *cmd_font_fullnames[]={
        "none","Classic","Size 8" };

const char *object_available_shortnames[]={
        "None", "Rsch", "Got",};

short script_param_to_int(int *val,const char *param)
{
  if ((param==NULL)||(val==NULL)) return false;
  int n_read;
  if (strncmp(param,"0x",2)==0)
  {
    n_read=sscanf(param+2,"%x",val);
  } else
  {
    n_read=sscanf(param,"%d",val);
  }
  return (n_read==1);
}

short script_param_to_ulong(unsigned long *val,const char *param)
{
  if ((param==NULL)||(val==NULL)) return false;
  int n_read;
  if (strncmp(param,"0x",2)==0)
  {
    n_read=sscanf(param+2,"%lx",val);
  } else
  {
    n_read=sscanf(param,"%lu",val);
  }
  return (n_read==1);
}

/*
 * Executes command from the CMD_ADIKTED group
 */
short execute_adikted_command(struct LEVEL *lvl,struct DK_SCRIPT_COMMAND *cmd,char *err_msg)
{
    char text[READ_BUFSIZE];
    switch (cmd->index)
    {
    case CUSTOM_COLUMN:
       if (cmd->param_count<14)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       int sx=-1,sy=-1;
       short result=true;
       result&=script_param_to_int(&sx,cmd->params[0]);
       result&=script_param_to_int(&sy,cmd->params[1]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read map subtile coordinates");
         return false;
       }
       int wib_val;
       int lintel;
       int orient;
       int base;
       result&=script_param_to_int(&wib_val,cmd->params[2]);
       result&=script_param_to_int(&lintel,cmd->params[3]);
       result&=script_param_to_int(&orient,cmd->params[4]);
       result&=script_param_to_int(&base,cmd->params[5]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read basic column parameters");
         return false;
       }
       int c[8];
       int i;
       for (i=0;i<8;i++)
           result&=script_param_to_int(&c[i],cmd->params[6+i]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read column cubes");
         return false;
       }
       /*Creating and filling custom column */
       struct DK_CUSTOM_CLM *ccol;
       ccol=create_cust_col();
       struct COLUMN_REC *clm_rec=ccol->rec;
       ccol->wib_val=wib_val;
       fill_column_rec_sim(clm_rec, 0, base,
           c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7]);
       clm_rec->lintel=lintel;
       clm_rec->orientation=orient;
       /*Adding custom column to level */
       cust_col_add_or_update(lvl,sx,sy,ccol);
       return true;
    case DEFINE_GRAFFITI:
      {
       if (cmd->param_count<7)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       int tx=-1,ty=-1,height=0;
       short result=true;
       result&=script_param_to_int(&tx,cmd->params[0]);
       result&=script_param_to_int(&ty,cmd->params[1]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read map tile coordinates");
         return false;
       }
       if (sscanf(cmd->params[2],"%d",&height)<1)
       {
         sprintf(err_msg,"Cannot read height in tile");
         return false;
       }
       int orient;
       orient=orient_cmd_index(cmd->params[3]);
       int font;
       font=font_cmd_index(cmd->params[4]);
       if (orient<0) orient=ORIENT_NS;
       if (font<0) font=GRAFF_FONT_ADICLSSC;
       int i,textlen;
       unsigned int cube=0x0184;
       /*Note: +2 is to skip 0x; must change this later... */
       if (sscanf(cmd->params[5]+2,"%x",&cube)<1)
       {
         sprintf(err_msg,"Cannot read filler cube index");
         return false;
       }
       textlen=strlen(cmd->params[6]);
       if (textlen>=READ_BUFSIZE) textlen=READ_BUFSIZE-1;
       for (i=0;i<textlen;i++)
       {
         char c=cmd->params[6][i];
         if ((c<=32)||(c=='_')) text[i]=' '; else
         if (c=='.') text[i]=','; else
         if (c=='{') text[i]='('; else
         if (c=='}') text[i]=')'; else
           text[i]=c;
       }
       text[textlen]='\0';
       int graf_idx=graffiti_add(lvl,tx,ty,height,text,font,orient,cube);
       if (graf_idx<0)
       {
         sprintf(err_msg,"Cannot add graffiti");
         return false;
       }
      };return true;
    case LEVEL_TIMESTAMP:
      {
       if (cmd->param_count<2)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       short result=true;
       unsigned long creat_date;
       unsigned long lastsav_date;
       result&=script_param_to_ulong(&creat_date,cmd->params[0]);
       result&=script_param_to_ulong(&lastsav_date,cmd->params[1]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read level date parameters");
         return false;
       }
       lvl->info.creat_date=creat_date;
       lvl->info.lastsav_date=lastsav_date;
      };return true;
    case USER_CMNDS_COUNT:
      {
       if (cmd->param_count<1)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       short result=true;
       unsigned long ucmdcnt;
       unsigned long umdswcnt,umslbcnt,ucrobcnt;
       result&=script_param_to_ulong(&ucmdcnt,cmd->params[0]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read first user commands count parameter");
         return false;
       }
       lvl->info.usr_cmds_count=ucmdcnt;
       /* The rest of parameters don't have to be present */
       if (cmd->param_count>3)
       {
         result&=script_param_to_ulong(&umdswcnt,cmd->params[1]);
         result&=script_param_to_ulong(&umslbcnt,cmd->params[2]);
         result&=script_param_to_ulong(&ucrobcnt,cmd->params[3]);
         if (!result)
         {
           sprintf(err_msg,"Cannot read user commands count parameters");
           return false;
         }
         lvl->info.usr_mdswtch_count=umdswcnt;
         lvl->info.usr_slbchng_count=umslbcnt;
         lvl->info.usr_creatobj_count=ucrobcnt;
       }
      };return true;
    case LEVEL_VERSION:
      {
       if (cmd->param_count<3)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       short result=true;
       int vma,vmi,vre;
       result&=script_param_to_int(&vma,cmd->params[0]);
       result&=script_param_to_int(&vmi,cmd->params[1]);
       result&=script_param_to_int(&vre,cmd->params[2]);
       if (!result)
       {
         sprintf(err_msg,"Cannot read version parameters");
         return false;
       }
       lvl->info.ver_major=vma;
       lvl->info.ver_minor=vmi;
       lvl->info.ver_rel=vre;
      };return true;
    case LEVEL_NAME:
      {
       if (cmd->param_count<1)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       char *levname=strdup_noquot(cmd->params[0]);
       if (levname!=NULL)
           set_lif_name_text(lvl,levname);
       /*int pos=0;*/
      };return true;
    case LEVEL_DESC:
      {
       if (cmd->param_count<1)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       free(lvl->info.desc_text);
       lvl->info.desc_text=strdup_noquot(cmd->params[0]);
      };return true;
    case LEVEL_AUTHORS:
      {
       if (cmd->param_count<2)
       {
         sprintf(err_msg,"%s requires more parameters",adikted_cmd_text(cmd->index));
         return false;
       }
       free(lvl->info.author_text);
       lvl->info.author_text=strdup_noquot(cmd->params[0]);
       free(lvl->info.editor_text);
       lvl->info.editor_text=strdup_noquot(cmd->params[1]);
      };return true;
    default:
      {
         sprintf(err_msg,"Internal command unrecognized");
      };return false;
    }
}

short script_cmd_verify_argcount(char *err_msg,const struct DK_SCRIPT_COMMAND *cmd,const int prop_count)
{
    if (cmd->param_count!=prop_count)
    {
      if (prop_count>0)
      {
        const char *cmd_text=script_cmd_text(cmd->group,cmd->index,"(unknown)");
        if (strlen(cmd_text) <= 16)
          sprintf(err_msg,"Script command \"%s\" requires %d arguments, has %d",
            cmd_text,prop_count,cmd->param_count);
        else
          sprintf(err_msg,"Script command requires %d arguments, has %d",
            prop_count,cmd->param_count);
/* old message - was too long
        char *amount;
        if (cmd->param_count>prop_count)
          amount="Too many";
        else
          amount="Not enough";
        sprintf(err_msg,"%s arguments (%d) for %s script command (%d required)",amount,
          cmd->param_count,script_cmd_text(cmd->group,cmd->index),prop_count);
*/
        return false;
      } else
      {
        const char *cmd_text=script_cmd_text(cmd->group,cmd->index,"(unknown)");
        if (strlen(cmd_text) <= 16)
          sprintf(err_msg,"Script command \"%s\" shouldn't have arguments",
              cmd_text);
        else
          sprintf(err_msg,"Script command shouldn't have arguments");
        return false;
      }
    }
    return true;
}

short string_is_decimal_number(const char *str,const short allow_sign)
{
  if (str==NULL) return false;
  if (str[0]=='\0') return false;
  int len=strlen(str);
  if (!isdigit(str[0]))
  {
    if ((!allow_sign)||(len<2)||((str[0]!='+')&&(str[0]!='-')))
      return false;
  }
  int i;
  for (i=1;i<len;i++)
  {
    if (!isdigit(str[i]))
        return false;
  }
  return true;
}

short script_cmd_verify_arg_actnpt(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,const short allow_herogate,const short allow_plyrheart)
{
    if (allow_plyrheart)
    {
      int cmd_idx;
      cmd_idx=players_cmd_index(param);
      if (cmd_idx>=0)
        return true;
    }
    if (!string_is_decimal_number(param,true))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid action point number \"%s\" in script",param);
        else
          sprintf(err_msg,"Invalid action point number in script");
        return false;
    }
    if (strlen(param)>6)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Too long action point number \"%s\" in script",param);
        else
          sprintf(err_msg,"Too long action point number in script");
        return false;
    }
    if ((!allow_herogate)&&(!string_is_decimal_number(param,false)))
    {
        sprintf(err_msg,"Hero gate not allowed as action point in script");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for in script");
        return false;
    }
    if (val>0)
    {
        if (val >= (scverif->actnpts_count))
        {
            sprintf(err_msg,"Too large %s number %d used in script","action point",val);
            return false;
        }
        if ((scverif->actnpts[val]) == 0)
        {
            sprintf(err_msg,"Nonexisting %s %d used in script","action point",val);
            return false;
        }
    } else
    if (val<0)
    {
        if ((-val) >= (scverif->herogts_count))
        {
            sprintf(err_msg,"Too large %s number %d used in script","hero gate",(-val));
            return false;
        }
        if ((scverif->herogts[-val]) == 0)
        {
            sprintf(err_msg,"Nonexisting %s %d used in script","hero gate",(-val));
            return false;
        }
    } else
    {
        sprintf(err_msg,"Zero is not valid action point number in script");
        return false;
    }
    return true;
}

short script_cmd_verify_arg_gameturn(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param)
{
    if (!string_is_decimal_number(param,false))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid game turns amount \"%s\" in script",param);
        else
          sprintf(err_msg,"Invalid game turns amount in script");
        return false;
    }
    if (strlen(param)>7)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Game turns amount \"%s\" too long in script",param);
        else
          sprintf(err_msg,"Game turns amount too long in script");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for param in script");
        return false;
    }
    if (val<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Game turns amout \"%s\" exceeds integer range in script",param);
        else
          sprintf(err_msg,"Game turns amout exceeds integer range in script");
        return false;
    }
    if (val > (20*60*60*24))
    {
            sprintf(err_msg,"Game turns amout %d exceeds one day delay in script",val);
            return false;
    }
    return true;
}

short script_cmd_verify_arg_goldamnt(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param)
{
    if (!string_is_decimal_number(param,false))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid gold amount \"%s\" in script",param);
        else
          sprintf(err_msg,"Invalid gold amount in script");
        return false;
    }
    if (strlen(param)>10)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Gold amount \"%s\" too long in script",param);
        else
          sprintf(err_msg,"Gold amount too long in script");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for param in script");
        return false;
    }
    if (val<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Gold amout \"%s\" exceeds integer range in script",param);
        else
          sprintf(err_msg,"Gold amout exceeds integer range in script");
        return false;
    }
    if (val > 2000000000)
    {
        sprintf(err_msg,"Gold amout %d exceeds safe value in script",val);
        return false;
    }
    return true;
}

short script_cmd_verify_arg_experience(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param)
{
    if (!string_is_decimal_number(param,false))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid experience level \"%s\" in script",param);
        else
          sprintf(err_msg,"Invalid experience level in script");
        return false;
    }
    if (strlen(param)>2)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Experience level \"%s\" too long in script",param);
        else
          sprintf(err_msg,"Experience level too long in script");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for param in script");
        return false;
    }
    if (val>10)
    {
        sprintf(err_msg,"Experience level %d exceeds maximum of %u in script",val,10);
        return false;
    }
    if (val < 1)
    {
        sprintf(err_msg,"Experience level must be at least 1, but is %d in script",val);
        return false;
    }
    return true;
}

short script_cmd_verify_arg_ncrtrs(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,int min_val,__attribute__((unused)) int *count)
{
    if (!string_is_decimal_number(param,false))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid creatures count \"%s\" in script",param);
        else
          sprintf(err_msg,"Invalid creatures count in script");
        return false;
    }
    if (strlen(param)>3)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Creatures count \"%s\" too long in script",param);
        else
          sprintf(err_msg,"Creatures count too long in script");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for param in script");
        return false;
    }
    if (val > 255)
    {
            sprintf(err_msg,"Creatures count %d exceeds maximum of %u in script",val,255);
            return false;
    }
    if (val < min_val)
    {
            sprintf(err_msg,"Creatures count must be at least %d, but is %d in script",min_val,val);
            return false;
    }
    return true;
}

short script_cmd_verify_arg_operator(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,int *opertr_idx)
{
    int cmd_idx;
    cmd_idx=operator_cmd_index(param);
    if (cmd_idx<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Unrecognized operator \"%s\" in script",param);
        else
          sprintf(err_msg,"Unrecognized operator in script");
        return false;
    }
    *opertr_idx=cmd_idx;
    return true;
}

short script_cmd_verify_arg_conditvar(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,short allow_vars,int *cmd_type,int *var_idx)
{
    (*cmd_type)=-1;
    int cmd_idx;
    if (allow_vars)
    {
      cmd_idx=variabl_cmd_index(param);
      if (cmd_idx>=0)
      {
        (*var_idx)=cmd_idx;
        (*cmd_type)=CMD_VARIBL;
      }
      cmd_idx=timer_cmd_index(param);
      if (cmd_idx>=0)
      {
        (*var_idx)=cmd_idx;
        (*cmd_type)=CMD_TIMER;
      }
      cmd_idx=flag_cmd_index(param);
      if (cmd_idx>=0)
      {
        (*var_idx)=cmd_idx;
        (*cmd_type)=CMD_FLAG;
      }
    } else
    {
      cmd_idx=spell_cmd_index(param);
      if (cmd_idx>=0)
      {
        (*var_idx)=cmd_idx;
        (*cmd_type)=CMD_SPELL;
      }
    }
    cmd_idx=creatures_cmd_index(param);
    if (cmd_idx>=0)
    {
      (*var_idx)=cmd_idx;
      (*cmd_type)=CMD_CREATR;
    }
    cmd_idx=door_cmd_index(param);
    if (cmd_idx>=0)
    {
      (*var_idx)=cmd_idx;
      (*cmd_type)=CMD_DOOR;
    }
    cmd_idx=trap_cmd_index(param);
    if (cmd_idx>=0)
    {
      (*var_idx)=cmd_idx;
      (*cmd_type)=CMD_TRAP;
    }
    cmd_idx=room_cmd_index(param);
    if (cmd_idx>=0)
    {
      (*var_idx)=cmd_idx;
      (*cmd_type)=CMD_ROOM;
    }
    if ((*cmd_type)<0)
    {
        char *whatis;
        if (allow_vars)
          whatis="variable";
        else
          whatis="name";
        if (strlen(param) <= 16)
          sprintf(err_msg,"Unrecognized %s \"%s\" in script",whatis,param);
        else
          sprintf(err_msg,"Unrecognized %s in script",whatis);
        return false;
    }
    return true;
}

short script_cmd_verify_arg_singlevar(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,char *param_name,func_cmd_index f_cmd_index,int *res_idx)
{
    int cmd_idx;
    cmd_idx=f_cmd_index(param);
    if (cmd_idx<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Unrecognized %s \"%s\" in script",param_name,param);
        else
          sprintf(err_msg,"Unrecognized %s in script",param_name);
        return false;
    }
    (*res_idx)=cmd_idx;
    return true;
}

short script_cmd_verify_arg_btnparam(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param)
{
    int cmd_idx;
    cmd_idx=players_cmd_index(param);
    if (cmd_idx>=0)
        return true;
    if (!string_is_decimal_number(param,true))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid %s value \"%s\" in script","player/time",param);
        else
          sprintf(err_msg,"Invalid %s value in script","player/time");
        return false;
    }
    const int max_digits=7;
    if (strlen(param)>max_digits)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"%s value \"%s\" too long in script","Game turns",param);
        else
          sprintf(err_msg,"%s value too long in script","Game turns");
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        else
          sprintf(err_msg,"Internal - no numeric value for param in script");
        return false;
    }
    if (val<-1)
    {
        if (strlen(param) <= 16)
            sprintf(err_msg,"%s amout \"%s\" exceeds range in script","Game turns",param);
        else
            sprintf(err_msg,"%s amout exceeds range in script","Game turns");
        return false;
    }
    if (val > (20*60*60*24))
    {
            sprintf(err_msg,"%s amout %d exceeds one day delay in script","Game turns",val);
            return false;
    }
    return true;
}


short script_cmd_verify_arg_creatr(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,int *crtr_idx)
{
    int cmd_idx;
    cmd_idx=creatures_cmd_index(param);
    if (cmd_idx<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Unrecognized creature \"%s\" in script",param);
        else
          sprintf(err_msg,"Unrecognized creature in script");
        return false;
    }
    (*crtr_idx)=cmd_idx;
    return true;
}

short script_cmd_verify_arg_party_objectv(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,const short heading_objctv,int *obj_idx)
{
    int cmd_idx;
    cmd_idx=party_objectv_cmd_index(param);
    char *whos;
    if (heading_objctv)
        whos="tunneller";
    else
        whos="party member";
    if (cmd_idx<0)
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Unrecognized %s objective \"%s\" in script",whos,param);
        else
          sprintf(err_msg,"Unrecognized %s objective in script",whos);
        return false;
    }
    if (((heading_objctv)&&(cmd_idx>OBJCTV_APPROP_DUNG))||
       ((!heading_objctv)&&(cmd_idx<=OBJCTV_APPROP_DUNG)))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Parameter \"%s\" is not %s objective in script",param,whos);
        else
          sprintf(err_msg,"Parameter is not %s objective in script",whos);
        return false;
    }
    (*obj_idx)=cmd_idx;
    return true;
}

short script_cmd_verify_arg_party_name(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,const short create_new,int *party_idx)
{
    int i;
    if (create_new)
    {
        i=0;
        while (i<MAX_PARTYS)
        {
            if (scverif->partys[i]==NULL) break;
            i++;
        }
        if (i>=MAX_PARTYS)
        {
            sprintf(err_msg,"Amount of partys in script exceeds %d",MAX_PARTYS);
            return false;
        }
        scverif->partys[i]=strdup(param);
        (*party_idx)=i;
    } else
    {
        i=0;
        while (i<MAX_PARTYS)
        {
            if (scverif->partys[i]!=NULL)
            {
                if (stricmp(param,scverif->partys[i])==0)
                  break;
            }
            i++;
        }
        if (i>=MAX_PARTYS)
        {
            if (strlen(param) <= 16)
              sprintf(err_msg,"Party \"%s\" used before definition in script",param);
            else
              sprintf(err_msg,"Party used before definition in script");
            return false;
        }
        (*party_idx)=i;
    }
    return true;
}

short script_cmd_verify_arg_quotparam(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif, char *err_msg,
        const char *param, __attribute__((unused)) const char *param_name, __attribute__((unused)) unsigned int *par_idx)
{
    int parlen=strlen(param);
    if (parlen<3)
    {
        sprintf(err_msg,"Text parameter %s too short to be in quote",param);
        return false;
    }
    /*short quote_sgl=((param[0]=='\'')&&(param[parlen-1]=='\'')); */
    short quote_dbl=((param[0]=='\"')&&(param[parlen-1]=='\"'));
    if ((!quote_dbl))/*(!quote_sgl)&& */
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Text parameter %s should be closed in quote",param);
        else
          sprintf(err_msg,"Text parameter should be closed in quote");
        return false;
    }
    if (strlen(param) > 511)
    {
        sprintf(err_msg,"Text parameter is longer than 511 chars");
        return false;
    }
    return true;
}

short script_cmd_verify_arg_limparam(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,const char *param_name,int min_val,unsigned int max_val)
{
    if (!string_is_decimal_number(param,false))
    {
        if (strlen(param) <= 16)
          sprintf(err_msg,"Invalid %s value \"%s\" in script",param_name,param);
        else
          sprintf(err_msg,"Invalid %s value in script",param_name);
        return false;
    }
    int max_digits=1;
    unsigned int tmp=max_val;
    while (true)
    {
        tmp=tmp/10;
        if (tmp>0)
          max_digits++;
        else
          break;
    }
    if (strlen(param)>max_digits)
    {
        sprintf(err_msg,"%s value \"%s\" too long in script",param_name,param);
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        return false;
    }
    if (val>max_val)
    {
        sprintf(err_msg,"%s value %d exceeds maximum of %u in script",param_name,val,max_val);
        return false;
    }
    if (val < min_val)
    {
        sprintf(err_msg,"%s value must be at least %d, but is %d in script",param_name,min_val,val);
        return false;
    }
    return true;
}

short script_cmd_verify_arg_unsg_int(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,int min_val,unsigned int max_val)
{
    if (!string_is_decimal_number(param,false))
    {
        sprintf(err_msg,"Invalid integer value \"%s\" in script",param);
        return false;
    }
    int max_digits=1;
    unsigned int tmp=max_val;
    while (true)
    {
        tmp=tmp/10;
        if (tmp>0)
          max_digits++;
        else
          break;
    }
    if (strlen(param)>max_digits)
    {
        sprintf(err_msg,"Integer value \"%s\" too long in script",param);
        return false;
    }
    int val;
    if (!script_param_to_int(&val,param))
    {
        sprintf(err_msg,"Internal - no numeric value for \"%s\" in script",param);
        return false;
    }
    if (val<0)
    {
            sprintf(err_msg,"Value \"%s\" exceeds integer range in script",param);
            return false;
    }
    if (val > max_val)
    {
            sprintf(err_msg,"Integer value %d exceeds maximum of %u in script",val,max_val);
            return false;
    }
    if (val < min_val)
    {
            sprintf(err_msg,"Value %d is smaller than minimum of %u in script",val,min_val);
            return false;
    }
    return true;
}

short script_cmd_verify_arg_logic_int(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,short *logic_val)
{
    if (strlen(param)>1)
    {
        sprintf(err_msg,"Logic integer value \"%s\" too long in script",param);
        return false;
    }
    if (param[0]=='0')
    {
      *logic_val=false;
      return true;
    }
    if (param[0]=='1')
    {
      *logic_val=true;
      return true;
    }
    sprintf(err_msg,"Logic value can only be 0 or 1, not \"%s\" in script",param);
    return false;
}

short script_cmd_verify_arg_specval(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,
        const char *param,char def_val)
{
    if (strlen(param)>1)
    {
        sprintf(err_msg,"Unused value \"%s\" should be \"%c\" in script",param,def_val);
        return false;
    }
    if (param[0]==def_val)
      return true;
    sprintf(err_msg,"Unused value \"%s\" should be \"%c\" in script",param,def_val);
    return false;
}

short script_cmd_verify_condit(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,
    char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int plyr_idx;
    int opertr_idx;
    int cmd_type;
    int cmd_idx;
    int i;
    short logic_val;
    switch (cmd->index)
    {
    case COND_IF:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_conditvar(scverif,err_msg,cmd->params[1],true,
            &cmd_type,&cmd_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_operator(scverif,err_msg,cmd->params[2],&opertr_idx))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }

        switch (cmd_type)
        {
        case CMD_CREATR:
          if (!script_cmd_verify_arg_ncrtrs(scverif,err_msg,cmd->params[3],0,&i))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_TIMER:
          if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[3]))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_FLAG:
          if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Flag",0,255))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_DOOR:
          if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Doors count",0,32767))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_TRAP:
          if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Traps count",0,32767))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_ROOM:
          if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Room slabs",0,7225))
          {
              (*err_param)=3;
              return VERIF_WARN;
          }
          break;
        case CMD_VARIBL:
          switch (cmd_idx)
          {
          case CRTRS_SCAVNG_GAINED:
          case CRTRS_SCAVNG_LOST:
          case CREATRS_ANNOYED:
          case TOTAL_CREATRS:
          case TOTAL_IMPS:
          case TOTAL_CREATRS_LEFT:
            if (!script_cmd_verify_arg_ncrtrs(scverif,err_msg,cmd->params[3],0,&i))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case MONEY:
          case TOTAL_GOLD_MINED:
            if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[3]))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case TOTAL_AREA:
          case ROOMS_DESTROYED:
          case DOORS_DESTROYED:
          case TOTAL_DOORS:
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Slabs count",0,7225))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case GAME_TURN:
            if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[3]))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case ALL_DNGNS_DESTROYED:
          case DNGN_DESTROYED:
            if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[3],&logic_val))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          /* Treat other parameters just as any integer */
          case TIMES_BROKEN_INTO:
          case BREAK_IN:
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Walls break",0,1024))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case SPELLS_STOLEN:
          case GOLD_POTS_STOLEN:
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Stolen items",0,1024))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case BATTLES_WON:
          case BATTLES_LOST:
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Battles count",0,1024))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          case TOTAL_RESEARCH:
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],"Battles count",0,2147483647))
            {
              (*err_param)=3;
              return VERIF_WARN;
            }
            break;
          default:
            sprintf(err_msg,"Internal problem with variable index in script");
            (*err_param)=1;
            return VERIF_WARN;
          }
          break;
        default:
            sprintf(err_msg,"Internal problem with variable in script");
            (*err_param)=1;
            return VERIF_WARN;
        }
        scverif->level++;
        scverif->total_ifs++;
        break;
    case IF_AVAILABLE:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_conditvar(scverif,err_msg,cmd->params[1],false,
            &cmd_type,&cmd_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_operator(scverif,err_msg,cmd->params[2],&opertr_idx))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[3],&logic_val))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        scverif->level++;
        scverif->total_ifs++;
        break;
    case IF_ACTNPT:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[0],false,false))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        scverif->level++;
        scverif->total_ifs++;
        break;
    case COND_ENDIF:
        if (!script_cmd_verify_argcount(err_msg,cmd,0))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        scverif->level--;
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong CONDIT script command");
        return VERIF_WARN;
    }
    if (scverif->level<0)
    {
        sprintf(err_msg,"Script command ENDIF found without IF");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_party(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int i;
    int plyr_idx;
    int crtr_idx;
    int party_idx;
    switch (cmd->index)
    {
    case CREATE_PARTY:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_name(scverif,err_msg,cmd->params[0],
            true,&party_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case ADD_TUNNELER_TOLEV:
        if (!script_cmd_verify_argcount(err_msg,cmd,6))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[1],true,true))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_objectv(scverif,err_msg,cmd->params[2],true,&i))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        switch (i)
        {
        case OBJCTV_ACTNPT:
            if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[3],true,true))
            {
                (*err_param)=3;
                return VERIF_WARN;
            }
            break;
        case OBJCTV_DUNGEON:
        case OBJCTV_DNHEART:
             /* In these cases, [head for] stores player index */
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],
                "Player index",0,5))
            /*if (!script_cmd_verify_arg_unsg_int(scverif,err_msg,cmd->params[3],0,5)) */
            {
                (*err_param)=3;
                return VERIF_WARN;
            }
            break;
        case OBJCTV_APPROP_DUNG:
            /* [head for] is not used for heading to appropiate undeon */
            if (!script_cmd_verify_arg_specval(scverif,err_msg,cmd->params[3],'0'))
            {
                (*err_param)=3;
                return VERIF_WARN;
            }
            break;
        default:
            sprintf(err_msg,"Internal problem with party objective in script");
            (*err_param)=3;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_experience(scverif,err_msg,cmd->params[4]))
        {
            (*err_param)=4;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_goldamnt(scverif,err_msg,cmd->params[5]))
        {
            (*err_param)=5;
            return VERIF_WARN;
        }
        break;
    case ADD_TO_PARTY:
        if (!script_cmd_verify_argcount(err_msg,cmd,6))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_name(scverif,err_msg,cmd->params[0],
            false,&party_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[1],&crtr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_experience(scverif,err_msg,cmd->params[2]))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_goldamnt(scverif,err_msg,cmd->params[3]))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_objectv(scverif,err_msg,cmd->params[4],false,&i))
        {
            (*err_param)=4;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[5]))
        {
            (*err_param)=5;
            return VERIF_WARN;
        }
        break;
    case ADD_PARTY_TOLEV:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_name(scverif,err_msg,cmd->params[1],
            false,&party_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[2],true,true))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_unsg_int(scverif,err_msg,cmd->params[3],1,200))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    case ADD_CREATR_TOLEV:
        if (!script_cmd_verify_argcount(err_msg,cmd,6))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[1],&crtr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[2],true,true))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_ncrtrs(scverif,err_msg,cmd->params[3],1,&i))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_experience(scverif,err_msg,cmd->params[4]))
        {
            (*err_param)=4;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_goldamnt(scverif,err_msg,cmd->params[5]))
        {
            (*err_param)=5;
            return VERIF_WARN;
        }
        break;
    case ADD_TUNNELLER_PARTY_TOLEV:
        if (!script_cmd_verify_argcount(err_msg,cmd,7))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_name(scverif,err_msg,cmd->params[1],
            false,&party_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[2],true,true))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_party_objectv(scverif,err_msg,cmd->params[3],true,&i))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        switch (i)
        {
        case OBJCTV_ACTNPT:
            if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[4],true,true))
            {
                (*err_param)=4;
                return VERIF_WARN;
            }
            break;
        case OBJCTV_DUNGEON:
        case OBJCTV_DNHEART:
             /* In these cases, [head for] stores player index */
            if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[4],
                "Player index",0,5))
            /*if (!script_cmd_verify_arg_unsg_int(scverif,err_msg,cmd->params[4],0,5)) */
            {
                (*err_param)=4;
                return VERIF_WARN;
            }
            break;
        case OBJCTV_APPROP_DUNG:
            if (!script_cmd_verify_arg_specval(scverif,err_msg,cmd->params[4],'0'))
            {
                (*err_param)=4;
                return VERIF_WARN;
            }
            break;
        default:
            sprintf(err_msg,"Internal problem with party objective in script");
            (*err_param)=4;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_experience(scverif,err_msg,cmd->params[5]))
        {
            (*err_param)=5;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_goldamnt(scverif,err_msg,cmd->params[6]))
        {
            (*err_param)=6;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong PARTY script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_avail(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int plyr_idx;
    int crtr_idx;
    int i;
    short logic_val;
    switch (cmd->index)
    {
    case ROOM_AVAIL:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "room",room_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[2],&logic_val))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[3],&logic_val))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    case CREATR_AVAIL:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[1],&crtr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[2],&logic_val))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[3],&logic_val))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    case MAGIC_AVAIL:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
         }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "spell",spell_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[2],&logic_val))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[3],&logic_val))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    case TRAP_AVAIL:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "trap",trap_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
         if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[2],&logic_val))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],
            "Traps amount",0,32767))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
       break;
    case DOOR_AVAIL:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "door",door_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[2],&logic_val))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],
            "Doors amount",0,32767))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong AVAIL script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_custobj(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int plyr_idx;
    int i;
    switch (cmd->index)
    {
    case DISPLAY_OBJECTV:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Message index",0,1023))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case DISPLAY_OBJECTV_WPOS:
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Message index",0,1023))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],
            "Map X coord",0,84))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[2],
            "Map Y coord",0,84))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        break;
    case DISPLAY_INFO:
        if (cmd->param_count!=2)
          if (!script_cmd_verify_argcount(err_msg,cmd,1))
          {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
          }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Message index",0,1023))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case DISPLAY_INFO_WPOS:
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Message index",0,1023))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],
            "Map X coord",0,84))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[2],
            "Map Y coord",0,84))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        break;
    case BONUS_LEV_TIME:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[0]))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case TUTORIAL_FLASH_BTN:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Button index",0,255))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_btnparam(scverif,err_msg,cmd->params[1]))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case QUICK_OBJECTV:
        if (cmd->param_count > 1)
          if (!script_cmd_verify_arg_quotparam(scverif,err_msg,cmd->params[1],
            "Objective text",&i))
          {
            (*err_param)=1;
            return VERIF_WARN;
          }
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Objective index",0,49))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[2],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        break;
    case QUICK_INFO:
        if (cmd->param_count > 1)
          if (!script_cmd_verify_arg_quotparam(scverif,err_msg,cmd->params[1],
            "Information text",&i))
          {
            (*err_param)=1;
            return VERIF_WARN;
          }
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[0],
            "Information index",0,49))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong CUSTOBJ script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_setup(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int obj_type;
    int i;
    int plyr_idx;
    int plyr2_idx;
    switch (cmd->index)
    {
    case SET_GEN_SPEED:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_gameturn(scverif,err_msg,cmd->params[0]))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case START_MONEY:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_goldamnt(scverif,err_msg,cmd->params[1]))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case COMP_PLAYER:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (plyr_idx==PLAYER0)
        {
          sprintf(err_msg,"%s is set as AI player in script",players_cmd_text(PLAYER0));
          (*err_param)=0;
          return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],
            "Computer player",0,10))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case ALLY_PLAYERS:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "player",players_cmd_index,&plyr2_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (plyr_idx==plyr2_idx)
        {
            sprintf(err_msg,"Cannot ally player with himself");
            (*err_param)=ERR_SCRIPTPARAM_WHOLE;
            return VERIF_WARN;
        }
        break;
    case SET_HATE:
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    case RESEARCH:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "object type",objtype_cmd_index,&obj_type))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        switch (obj_type)
        {
        case OBJTYPE_CREATURE:
            if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[2],&i))
            {
                (*err_param)=2;
                return VERIF_WARN;
            }
            break;
        case OBJTYPE_ROOM:
            if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[2],
                "room",room_cmd_index,&i))
            {
                (*err_param)=2;
                return VERIF_WARN;
            }
            break;
        case OBJTYPE_SPELL:
            if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[2],
                "spell",spell_cmd_index,&i))
            {
                (*err_param)=2;
                return VERIF_WARN;
            }
            break;
        default:
            sprintf(err_msg,"Internal problem with object type in script");
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[3],
            "Research points",1,16777216))
        {
            (*err_param)=3;
            return VERIF_WARN;
        }
        break;
    case SET_COMPUTER_GLOBALS:
        if (!script_cmd_verify_argcount(err_msg,cmd,7))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case SET_COMPUTER_CHECKS:
        if (!script_cmd_verify_argcount(err_msg,cmd,7))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case SET_COMPUTER_EVENT:
        if (!script_cmd_verify_argcount(err_msg,cmd,4))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case SET_COMPUTER_PROCESS:
        if (!script_cmd_verify_argcount(err_msg,cmd,7))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case MAX_CREATURES:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_ncrtrs(scverif,err_msg,cmd->params[1],0,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong SETUP script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_triger(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int plyr_idx;
    int i;
    switch (cmd->index)
    {
    case RESET_ACTNPT:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_actnpt(scverif,err_msg,cmd->params[0],false,false))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case NEXT_CMD_REUSABLE:
        if (!script_cmd_verify_argcount(err_msg,cmd,0))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    case SET_FLAG:
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "flag",flag_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[2],"Flag",0,255))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        break;
    case SET_TIMER:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[1],
            "timer",timer_cmd_index,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case GAME_LOSE:
        if (!script_cmd_verify_argcount(err_msg,cmd,0))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    case GAME_WIN:
        if (!script_cmd_verify_argcount(err_msg,cmd,0))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    case SET_MUSIC:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong TRIGER script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_crtradj(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    int i;
    int plyr_idx;
    int crtr_idx;
    int crtr2_idx;
    short logic_val;
    switch (cmd->index)
    {
    case DEAD_CREATURES_RET_TO_POOL:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_logic_int(scverif,err_msg,cmd->params[0],&logic_val))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        break;
    case ADD_CREATR_TO_POOL:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_ncrtrs(scverif,err_msg,cmd->params[1],0,&i))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        scverif->total_in_pool+=i;
        break;
    case SET_CREATR_MAX_LEVEL:
        if (!script_cmd_verify_argcount(err_msg,cmd,3))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_singlevar(scverif,err_msg,cmd->params[0],
            "player",players_cmd_index,&plyr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[1],&crtr_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_experience(scverif,err_msg,cmd->params[2]))
        {
            (*err_param)=2;
            return VERIF_WARN;
        }
        break;
    case SET_CREATR_STRENGTH:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],"Strength",0,255))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case SET_CREATR_HEALTH:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],"Health",0,7895))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case SET_CREATR_ARMOUR:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],"Armour",0,255))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case SET_CREATR_FEAR:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_limparam(scverif,err_msg,cmd->params[1],"Fear",0,255))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    case CREATR_SWAP:
        if (!script_cmd_verify_argcount(err_msg,cmd,2))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[0],&crtr_idx))
        {
            (*err_param)=0;
            return VERIF_WARN;
        }
        if (!script_cmd_verify_arg_creatr(scverif,err_msg,cmd->params[1],&crtr2_idx))
        {
            (*err_param)=1;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong CRTRADJ script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_commnt(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif, char *err_msg, __attribute__((unused)) int *err_param, const struct DK_SCRIPT_COMMAND *cmd)
{
    switch (cmd->index)
    {
    case EMPTYLN:
    case CMNT_REM:
        break;
    default:
        sprintf(err_msg,"Internal warn - wrong COMMNT script command");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

short script_cmd_verify_obsol(__attribute__((unused)) struct SCRIPT_VERIFY_DATA *scverif,char *err_msg,int *err_param,const struct DK_SCRIPT_COMMAND *cmd)
{
    switch (cmd->index)
    {
    case OBSOLT_PRINT:
        if (!script_cmd_verify_argcount(err_msg,cmd,1))
        {
            (*err_param)=ERR_SCRIPTPARAM_NARGS;
            return VERIF_WARN;
        }
        break;
    default:
        sprintf(err_msg,"Obsolete command used in script");
        return VERIF_WARN;
    }
    return VERIF_OK;
}

/*
 * Verifies TXT entries. Returns VERIF_ERROR, VERIF_WARN or VERIF_OK
 * This is just a wrapper to unify parameters in the main checking function.
 */
short txt_verify(const struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    int err_line=-1;
    int err_param=ERR_SCRIPTPARAM_WHOLE;
    short result=dkscript_verify(lvl,err_msg,&err_line,&err_param);
    if (result!=VERIF_OK)
    {
      errpt->x=-1;
      errpt->y=-1;
    }
    return result;
}

/*
 * Verifies TXT entries. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short dkscript_verify(const struct LEVEL *lvl, char *err_msg,int *err_line,int *err_param)
{
    char child_err_msg[LINEMSG_SIZE];
    child_err_msg[0]='\0';
    struct SCRIPT_VERIFY_DATA scverif;
    scverif.dnhearts_count=0;
    scverif.herogts_count=256;
    if (!create_herogate_number_used_arr(lvl,&(scverif.herogts),&(scverif.herogts_count)))
        sprintf(child_err_msg,"hero gates");
    scverif.actnpts_count=256;
    if (!create_actnpt_number_used_arr(lvl,&(scverif.actnpts),&(scverif.actnpts_count)))
        sprintf(child_err_msg,"action points");
    if (child_err_msg[0]!='\0')
    {
        sprintf(err_msg,"Internal - cannot list %s to verify script",child_err_msg);
        return VERIF_WARN;
    }
    scverif.level=0;
    scverif.total_ifs=0;
    scverif.total_in_pool=0;
    scverif.partys=(char **)malloc((MAX_PARTYS+1)*sizeof(char *));
    if (scverif.partys==NULL)
    {
      message_error("txt_verify: Cannot allocate memory");
      return VERIF_WARN;
    }
    int i;
    for (i=0; i<(MAX_PARTYS+1); i++)
      scverif.partys[i]=NULL;

    short result=VERIF_OK;
    /*Sweeping through TXT entries */
    for (i=0; i<lvl->script.lines_count; i++)
    {
        struct DK_SCRIPT_COMMAND *cmd;
        cmd=lvl->script.list[i];
        switch (cmd->group)
        {
        case CMD_CONDIT:
            result=script_cmd_verify_condit(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_PARTY:
            result=script_cmd_verify_party(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_AVAIL:
            result=script_cmd_verify_avail(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_CUSTOBJ:
            result=script_cmd_verify_custobj(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_SETUP:
            result=script_cmd_verify_setup(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_TRIGER:
            result=script_cmd_verify_triger(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_CRTRADJ:
            result=script_cmd_verify_crtradj(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_COMMNT:
            result=script_cmd_verify_commnt(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_OBSOLT:
            result=script_cmd_verify_obsol(&scverif,child_err_msg,err_param,cmd);
            break;
        case CMD_UNKNOWN:
            sprintf(child_err_msg,"Unrecognized script command");
            result=VERIF_WARN;
            break;
        case CMD_ADIKTED:
            sprintf(child_err_msg,"%s specific command used in DK script",PROGRAM_NAME);
            result=VERIF_WARN;
            break;
        default:
            sprintf(child_err_msg,"Internal - bad script command group");
            result=VERIF_WARN;
            break;
        }
        /* If error found - break the for loop */
        if (result!=VERIF_OK)
        {
            sprintf(err_msg,"%s at line %d.",child_err_msg,i+1);
            *err_line=i;
            break;
        }
    }
    const int max_condit_if=48;
    if ((result==VERIF_OK)&&(scverif.total_ifs>max_condit_if))
    {
        sprintf(err_msg,"Script file contains more than %d IF statements.",max_condit_if);
        result=VERIF_WARN;
    }
    if ((result==VERIF_OK)&&(scverif.level!=0))
    {
        if (scverif.level>0)
          sprintf(err_msg,"There are %d unclosed IF statements",scverif.level);
        else
          sprintf(err_msg,"Amount of ENDIFs is larger than of IF statements");
        result=VERIF_WARN;
    }
    /*Freeing structure and returning */
    for (i=0; i<16; i++)
      free(scverif.partys[i]);
    free(scverif.partys);
    free(scverif.actnpts);
    free(scverif.herogts);
    return result;
}

short execute_script_line(struct LEVEL *lvl,char *line,char *err_msg)
{
    struct DK_SCRIPT_COMMAND *cmd;
    cmd=malloc(sizeof(struct DK_SCRIPT_COMMAND));
    if (cmd==NULL)
    {
      message_error("Cannot allocate memory to execute script command");
      return false;
    }
    script_command_clear(cmd);
    int result;
    if (decompose_script_command(cmd,line,&(lvl->optns.script)))
    {
      switch (cmd->group)
      {
      case CMD_ADIKTED:
          result=execute_adikted_command(lvl,cmd,err_msg);
          break;
      case CMD_COMMNT:
          result=true;
          break;
      default:
          sprintf(err_msg,"Command is not executable in %s",PROGRAM_NAME);
          result=false;
          break;
      }
    } else
    {
      /*Check if it's just empty line */
      if ((line==NULL)||(strlen(line)<1))
      {
        result=true;
      } else
      {
        sprintf(err_msg,"Cannot identify command");
        result=false;
      }
    }
    script_command_free(cmd);
    return result;
}

short decompose_script(struct DK_SCRIPT *script,const struct SCRIPT_OPTIONS *optns)
{
  if (script==NULL) return false;
  message_log("  decompose_script: %d lines to analyze",script->lines_count);
  int i;
  for (i=0;i<script->lines_count;i++)
  {
      /*message_log("  decompose_script: line %3d",i); */
      struct DK_SCRIPT_COMMAND *cmd=script->list[i];
      script_command_renew(&cmd);
      decompose_script_command(cmd,script->txt[i],optns);
      /*This is needed because script_command_renew() could change the pointer */
      script->list[i]=cmd;
  }
  return true;
}

short recompose_script(struct DK_SCRIPT *script,const struct SCRIPT_OPTIONS *optns)
{
  if (script==NULL) return false;
  int i;
  for (i=0;i<script->lines_count;i++)
  {
      char *nline;
      nline=recompose_script_command(script->list[i],optns);
      free(script->txt[i]);
      if (nline!=NULL)
        script->txt[i]=nline;
      else
        script->txt[i]=strdup("");
  }
  return true;
}

short recompute_script_levels(struct DK_SCRIPT *script)
{
  if (script==NULL) return false;
  int i;
  int lev=0;
  short result=true;
  for (i=0;i<script->lines_count;i++)
  {
      struct DK_SCRIPT_COMMAND *cmd=script->list[i];
      switch (cmd->group)
      {
      case CMD_CONDIT:
        if (cmd->index==COND_ENDIF)
        {
          lev--;
          cmd->level=max(lev,0);
        } else
        {
          cmd->level=max(lev,0);
          lev++;
        }
        break;
      case CMD_PARTY:
        if (cmd->index==ADD_TO_PARTY)
          cmd->level=max(lev+1,1);
        else
          cmd->level=max(lev,0);
        break;
      default:
        cmd->level=max(lev,0);
        break;
      }
      if (lev<0)
        result=false;
  }
  if (lev!=0)
    result=false;
  return result;
}

int get_script_command_level(const char *text,const struct SCRIPT_OPTIONS *optns)
{
    int lev=0;
    int nspac=0;
    int len=strlen(text);
    int i;
    for (i=0;i<len;i++)
    {
        if (text[i]==' ')
        {
          nspac++;
          if (nspac>=optns->level_spaces)
          {
            nspac=0;
            lev++;
          }
        } else
        if (text[i]=='\t')
        {
            lev++;
        } else
        break;
    }
    if (nspac>0)
      lev++;
    return lev;
}

/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_condit(struct DK_SCRIPT_PARAMETERS *par,
    struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
    int plyr_idx;
    /*int conditvar_type;*/
    int conditvar_idx;
    /*int cmpvar_type;*/
    int cmpvar_idx;
    int opertr_idx;
    int actnpt_num;
    switch (cmd->index)
    {
    case COND_IF:
        par->end_level++;
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        opertr_idx=operator_cmd_index(cmd->params[2]);
        if ((plyr_idx<0)||(opertr_idx<0))
            return false;
        conditvar_idx=-1;
        cmpvar_idx=-1;
        /*conditvar_type=*/recognize_script_word_group_and_idx(&conditvar_idx,cmd->params[1],true);
        /*cmpvar_type=*/recognize_script_word_group_and_idx(&cmpvar_idx,cmd->params[3],true);
        if ((conditvar_idx<0)||(cmpvar_idx<0))
            return false;
/*TODO */
/* As for now, just add those commands to "rest" list. */
        break;
    case IF_AVAILABLE:
        par->end_level++;
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        opertr_idx=operator_cmd_index(cmd->params[2]);
        if ((plyr_idx<0)||(opertr_idx<0))
            return false;
        conditvar_idx=-1;
        cmpvar_idx=-1;
        /*conditvar_type=*/recognize_script_word_group_and_idx(&conditvar_idx,cmd->params[1],true);
        /*cmpvar_type=*/recognize_script_word_group_and_idx(&cmpvar_idx,cmd->params[3],true);
        if ((conditvar_idx<0)||(cmpvar_idx<0))
            return false;
/*TODO */
/* As for now, just add those commands to "rest" list. */
        break;
    case IF_ACTNPT:
        par->end_level++;
        if (cmd->param_count<2)
            return false;
        if (!script_param_to_int(&actnpt_num,cmd->params[0]))
            return false;
        plyr_idx=players_cmd_index(cmd->params[1]);
        if (plyr_idx<0)
            return false;
/*TODO */
/* As for now, just add those commands to "rest" list. */
        break;
    case COND_ENDIF:
        par->end_level--;
/*TODO */
/* As for now, just add those commands to "rest" list. */
        break;
    default:
        return false;
    }
  return true;
}

/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 * This one is used for commands inside a conditional loop
 */
short script_decomposed_to_params_cmd_blockbody(__attribute__((unused)) struct DK_SCRIPT_PARAMETERS *par,
    __attribute__((unused)) struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
/*TODO! */
/* As for now, just add those commands to "rest" list. */
  return true;
}

/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_party(__attribute__((unused)) struct DK_SCRIPT_PARAMETERS *par,
    __attribute__((unused)) struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
/*TODO! */
/* As for now, just add those commands to "rest" list. */
  return true;
}

/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_avail(struct DK_SCRIPT_PARAMETERS *par,
    struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
    int plyr_idx;
    int object_idx;
    int logic_val;
    int amount;
    short available;
    int i;
    switch (cmd->index)
    {
    case ROOM_AVAIL:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        object_idx=room_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(object_idx<0))
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[2]))
            return false;
        if (!script_param_to_int(&amount,cmd->params[3]))
            return false;
        if (amount)
            available=AVAIL_INSTANT;
        else
        if (logic_val)
            available=AVAIL_RESEARCH;
        else
            available=AVAIL_NO;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].room_avail[object_idx]=available;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
              par->player[i].room_avail[object_idx]=available;
        } else
        {
            return false;
        }
        break;
    case CREATR_AVAIL:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        object_idx=creatures_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(object_idx<0))
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[2]))
            return false;
        if (!script_param_to_int(&amount,cmd->params[3]))
            return false;
        if (amount)
            available=AVAIL_INSTANT;
        else
        if (logic_val)
            available=AVAIL_RESEARCH;
        else
            available=AVAIL_NO;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].creature_avail[object_idx]=available;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
              par->player[i].creature_avail[object_idx]=available;
        } else
        {
            return false;
        }
        break;
    case MAGIC_AVAIL:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        object_idx=spell_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(object_idx<0))
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[2]))
            return false;
        if (!script_param_to_int(&amount,cmd->params[3]))
            return false;
        if (amount)
            available=AVAIL_INSTANT;
        else
        if (logic_val)
            available=AVAIL_RESEARCH;
        else
            available=AVAIL_NO;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].spell_avail[object_idx]=available;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
              par->player[i].spell_avail[object_idx]=available;
        } else
        {
            return false;
        }
        break;
    case TRAP_AVAIL:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        object_idx=trap_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(object_idx<0))
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[2]))
            return false;
        if (!script_param_to_int(&amount,cmd->params[3]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].trap_avail[object_idx]=(logic_val!=0);
            par->player[plyr_idx].trap_amount[object_idx]=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
            {
              par->player[i].trap_avail[object_idx]=(logic_val!=0);
              par->player[i].trap_amount[object_idx]=amount;
            }
        } else
        {
            return false;
        }
       break;
    case DOOR_AVAIL:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        object_idx=door_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(object_idx<0))
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[2]))
            return false;
        if (!script_param_to_int(&amount,cmd->params[3]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].door_avail[object_idx]=(logic_val!=0);
            par->player[plyr_idx].door_amount[object_idx]=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
            {
              par->player[i].door_avail[object_idx]=(logic_val!=0);
              par->player[i].door_amount[object_idx]=amount;
            }
        } else
        {
            return false;
        }
        break;
    default:
        return false;
    }
  return true;
}
/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_custobj(__attribute__((unused)) struct DK_SCRIPT_PARAMETERS *par,
    __attribute__((unused)) struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
/*TODO */
/* As for now, just add those commands to "rest" list. */
  return true;
}
/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_setup(struct DK_SCRIPT_PARAMETERS *par,
    struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
    int i;
    int plyr_idx;
    int plyr2_idx;
    int amount;
    int type_idx;
    switch (cmd->index)
    {
    case SET_GEN_SPEED:
        if (cmd->param_count<1)
            return false;
        if (!script_param_to_int(&amount,cmd->params[0]))
            return false;
        par->portal_gen_speed=amount;
        break;
    case START_MONEY:
        if (cmd->param_count<2)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if (plyr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].start_gold=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
                par->player[i].start_gold=amount;
        } else
        {
            return false;
        }
        break;
    case COMP_PLAYER:
        if (cmd->param_count<2)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if (plyr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
           par->player[plyr_idx].computer_player=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
               par->player[i].computer_player=amount;
        } else
        {
            return false;
        }
        break;
    case ALLY_PLAYERS:
        if (cmd->param_count<2)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        plyr2_idx=players_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(plyr2_idx<0))
            return false;
        if ((plyr_idx<PLAYERS_COUNT)&&(plyr2_idx<PLAYERS_COUNT))
        {
            par->player[plyr_idx].ally[plyr2_idx]=1;
            par->player[plyr2_idx].ally[plyr_idx]=1;
        } else
        {
            return false;
        }
        break;
    case SET_HATE:
        if (cmd->param_count<3)
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case RESEARCH:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if (plyr_idx<0)
            return false;
        type_idx=objtype_cmd_index(cmd->params[1]);
        if (type_idx<0)
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case SET_COMPUTER_GLOBALS:
        if (cmd->param_count<7)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if ((plyr_idx<0))
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case SET_COMPUTER_CHECKS:
        if (cmd->param_count<7)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if ((plyr_idx<0))
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case SET_COMPUTER_EVENT:
        if (cmd->param_count<4)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if ((plyr_idx<0))
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case SET_COMPUTER_PROCESS:
        if (cmd->param_count<7)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if ((plyr_idx<0))
            return false;
        /*TODO */
/* As for now, just add those commands to "rest" list. */
        return false;
        break;
    case MAX_CREATURES:
        if (cmd->param_count<2)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        if ((plyr_idx<0))
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].max_creatures=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
                par->player[i].max_creatures=amount;
        } else
        {
            return false;
        }
        break;
    default:
        return false;
    }
  return true;
}
/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_triger(__attribute__((unused)) struct DK_SCRIPT_PARAMETERS *par,
    __attribute__((unused)) struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
/*TODO */
/* As for now, just add those commands to "rest" list. */
  return true;
}
/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd_crtradj(struct DK_SCRIPT_PARAMETERS *par,
    struct DK_SCRIPT_COMMAND *cmd, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
    int i;
    int plyr_idx;
    int crtr_idx;
    int crtr2_idx;
    int logic_val;
    int amount;
    switch (cmd->index)
    {
    case DEAD_CREATURES_RET_TO_POOL:
        if (cmd->param_count<1)
            return false;
        if (!script_param_to_int(&logic_val,cmd->params[0]))
            return false;
        par->dead_return_to_pool=(logic_val!=0);
        break;
    case ADD_CREATR_TO_POOL:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        if (crtr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        par->creature_pool[crtr_idx]=amount;
        break;
    case SET_CREATR_MAX_LEVEL:
        if (cmd->param_count<3)
            return false;
        plyr_idx=players_cmd_index(cmd->params[0]);
        crtr_idx=creatures_cmd_index(cmd->params[1]);
        if ((plyr_idx<0)||(crtr_idx<0))
            return false;
        if (!script_param_to_int(&amount,cmd->params[2]))
            return false;
        if (plyr_idx<PLAYERS_COUNT)
        {
            par->player[plyr_idx].creature_maxlvl[crtr_idx]=amount;
        } else
        if (plyr_idx==PLAYER_ALL)
        {
            for (i=0;i<PLAYERS_COUNT;i++)
              par->player[i].creature_maxlvl[crtr_idx]=amount;
        } else
        {
            return false;
        }
        break;
    case SET_CREATR_STRENGTH:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        if (crtr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        /*TODO */
        break;
    case SET_CREATR_HEALTH:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        if (crtr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        /*TODO */
        break;
    case SET_CREATR_ARMOUR:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        if (crtr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        /*TODO */
        break;
    case SET_CREATR_FEAR:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        if (crtr_idx<0)
            return false;
        if (!script_param_to_int(&amount,cmd->params[1]))
            return false;
        /*TODO */
        break;
    case CREATR_SWAP:
        if (cmd->param_count<2)
            return false;
        crtr_idx=creatures_cmd_index(cmd->params[0]);
        crtr2_idx=creatures_cmd_index(cmd->params[1]);
        if ((crtr_idx<0)||(crtr2_idx<0))
            return false;
        /*TODO */
        break;
    default:
        return false;
    }
  return true;
}

/*
 * Analyzes single decomposed script command and adjusts DK_SCRIPT_PARAMETERS;
 */
short script_decomposed_to_params_cmd(struct DK_SCRIPT_PARAMETERS *par,
    struct DK_SCRIPT_COMMAND *cmd,const struct SCRIPT_OPTIONS *optns)
{
  if (par->end_level==0)
  {
    switch (cmd->group)
    {
    case CMD_CONDIT:
            return script_decomposed_to_params_cmd_condit(par,cmd,optns);
    case CMD_PARTY:
            return script_decomposed_to_params_cmd_party(par,cmd,optns);
    case CMD_AVAIL:
            return script_decomposed_to_params_cmd_avail(par,cmd,optns);
    case CMD_CUSTOBJ:
            return script_decomposed_to_params_cmd_custobj(par,cmd,optns);
    case CMD_SETUP:
            return script_decomposed_to_params_cmd_setup(par,cmd,optns);
    case CMD_TRIGER:
            return script_decomposed_to_params_cmd_triger(par,cmd,optns);
    case CMD_CRTRADJ:
            return script_decomposed_to_params_cmd_crtradj(par,cmd,optns);
    case CMD_COMMNT:
            return true;
    case CMD_OBSOLT:
            return true;
    case CMD_UNKNOWN:
    case CMD_ADIKTED:
        default:
            return false;
    }
  } else
  {
      return script_decomposed_to_params_cmd_blockbody(par,cmd,optns);
  }
}

/*
 * Converts decomposed script commands into DK_SCRIPT_PARAMETERS;
 * automatically clears (but not allocates) DK_SCRIPT_PARAMETERS struct at start.
 */
short script_decomposed_to_params(struct DK_SCRIPT *script,const struct SCRIPT_OPTIONS *optns)
{
  if (script==NULL) return false;
  short result;
  /* Clearing the struct */
  result=level_clear_script_param(&(script->par));
  int i;
  /* Now filling */
  for (i=0;i<script->lines_count;i++)
  {
      /*message_log("  script_decomposed_to_params: recognizing line %d",i); */
      result&=script_decomposed_to_params_cmd(&(script->par),script->list[i],optns);
  }
  return result;
}

short script_params_to_decomposed(__attribute__((unused)) struct DK_SCRIPT *script, __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
/*TODO */
/* But before start working on this, I must make all script commands */
/* to be decomposed! */
  return false;
}

short script_strword_pos( char const **ptr, unsigned int *ptr_len, const char *str, const short whole_rest )
{
  static const char *text;
  if (str!=NULL)
  {
    text=str;
  }
  if (text==NULL)
  {
    (*ptr_len)=0;
/*    message_log("  script_strword_pos: end of text"); */
    return false;
  }
  int len;
  do {
    len=strcspn(text," \t,();");
    if (len==0)
    {
      if (text[0]=='\0')
      {
        (*ptr_len)=0;
        text=NULL;
/*          message_log("  script_strword_pos: line empty"); */
        return false;
      }
      text++;
    }
  } while (len==0);
  int text_len;
  text_len=strlen(text);
  if (whole_rest)
  {
    (*ptr)=text;
    (*ptr_len)=text_len;
/*    message_log("  script_strword_pos: returning whole \"%s\"",text); */
    text=NULL;
    return true;
  }
  /* So now we're sure that first character is not a token. */
  /* and that the string is not empty */
  /* Determining end of the parameter */
  /* operators */
  if ((text[0]=='=')||(text[0]=='>')||(text[0]=='<')
  ||(text[0]=='|')||(text[0]=='/')||(text[0]=='*')||(text[0]=='!'))
  {
    len=1;
    while (((text[len]=='=')||(text[len]=='>')||(text[len]=='<')||
        (text[len]=='|')||(text[len]=='/')||(text[len]=='*'))&&(len<3))
    {
      len++;
    }
  } else
  /* Text block taken into quote */
  if ((text[0]=='\"')||(text[0]=='\''))
  {
    char quot_chr[2];
    quot_chr[0]=text[0];
    quot_chr[1]='\0';
    len=1;
    do {
      len=strcspn( text+len, quot_chr );
      if (len<text_len) len++;
    } while ((len<text_len)&&(text[len-1]=='\\'));
    if (len<text_len) len++;
  } else
  /* Standard word */
  {
    /* Check for operators after the word */
    int oplen=0;
    if (text[0]!='\0')
      oplen=strcspn(text+1,"=><|/*\"\'-+!")+1;
    if (oplen<len)
      len=oplen;
  }
  (*ptr)=text;
  (*ptr_len)=len;
  text+=len;
  return true;
}

char *script_strword( const char *str, const short whole_rest )
{
  const char *ptr;
  unsigned int len;
  if (!script_strword_pos(&ptr,&len,str,whole_rest))
    return NULL;
  char *wordtxt;
  wordtxt=malloc(len+1);
  if (wordtxt==NULL)
  {
    message_error("script_strword: cannot allocate memory");
    return NULL;
  }
  strncpy(wordtxt,ptr,len);
  wordtxt[len]='\0';
  return wordtxt;
}

/*
 * Returns group and index of a script word
 */
int recognize_script_word_group_and_idx(int *index,const char *wordtxt,const short is_parameter)
{
  int cmd_idx;
  if (is_parameter)
  {
    cmd_idx=comp_plyr_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_COMP;
    }
    cmd_idx=players_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_PLAYER;
    }
    cmd_idx=operator_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_OPERATR;
    }
    cmd_idx=objtype_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_OBJTYPE;
    }
    cmd_idx=variabl_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_VARIBL;
    }
    cmd_idx=timer_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_TIMER;
    }
    cmd_idx=flag_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_FLAG;
    }
    cmd_idx=party_objectv_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_PAROBJ;
    }
    cmd_idx=creatures_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_CREATR;
    }
    cmd_idx=door_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_DOOR;
    }
    cmd_idx=trap_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_TRAP;
    }
    cmd_idx=spell_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_SPELL;
    }
    cmd_idx=room_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_ROOM;
    }
    cmd_idx=special_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_SPECIAL;
    }
  } else
  {
    cmd_idx=adikted_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_ADIKTED;
    }
    cmd_idx=condit_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_CONDIT;
    }
    cmd_idx=party_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_PARTY;
    }
    cmd_idx=avail_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_AVAIL;
    }
    cmd_idx=custobj_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_CUSTOBJ;
    }
    cmd_idx=setup_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_SETUP;
    }
    cmd_idx=triger_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_TRIGER;
    }
    cmd_idx=crtradj_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_CRTRADJ;
    }
    cmd_idx=obsolt_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_OBSOLT;
    }
    cmd_idx=commnt_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      *index=cmd_idx;
      return CMD_COMMNT;
    }
  }
  *index=-1;
  return CMD_UNKNOWN;
}

short decompose_script_command(struct DK_SCRIPT_COMMAND *cmd,const char *text,const struct SCRIPT_OPTIONS *optns)
{
  if ((cmd==NULL)||(text==NULL)) return false;
  /*Decomposing the string into single parameters - getting first (command name) */
  char *wordtxt;
  cmd->level=get_script_command_level(text,optns);
  wordtxt = script_strword(text,false);
  int cmd_idx;
  cmd->group=recognize_script_word_group_and_idx(&cmd_idx,wordtxt,false);
  cmd->index=cmd_idx;
  if (cmd->index<0)
  {
      cmd->group=CMD_UNKNOWN;
      message_log("  decompose_script_command: \"%s\" not recognized",wordtxt);
      free(wordtxt);
      script_command_param_add(cmd,strdup(text));
      return false;
  }
  free(wordtxt);
  /*Decomposing the string into parameters - in this case, treat the rest as one parameter */
  if (cmd->group==CMD_COMMNT)
  {
      wordtxt = script_strword(NULL,true);
/*      message_log("  decompose_script_command: got comment \"%s\"",wordtxt); */
      script_command_param_add(cmd,wordtxt);
  } else
  /*Decomposing the string into single parameters - getting the rest of parameters */
  {
    while (wordtxt != NULL)
    {
      wordtxt = script_strword(NULL,false);
/*      message_log("  decompose_script_command: got parameter \"%s\"",wordtxt); */
      script_command_param_add(cmd,wordtxt);
    }
  }
  return true;
}

/*
 * Adds param as next script command parameter for cmd; adds direct pointer,
 * without duplicatidg the param pointer.
 */
short script_command_param_add(struct DK_SCRIPT_COMMAND *cmd,char *param)
{
    if ((cmd==NULL)||(param==NULL)) return false;
    int param_idx=cmd->param_count;
    cmd->params=realloc(cmd->params,(param_idx+1)*sizeof(char *));
    cmd->params[param_idx]=param;
    cmd->param_count=param_idx+1;
    return true;
}

short is_no_bracket_command(int group,int cmdidx)
{
    switch (group)
    {
    case CMD_CONDIT:
      switch (cmdidx)
      {
      case COND_ENDIF:
        return true;
      }
      return false;
    case CMD_PARTY:
      return false;
    case CMD_AVAIL:
      return false;
    case CMD_CUSTOBJ:
      return false;
    case CMD_SETUP:
      return false;
    case CMD_TRIGER:
      switch (cmdidx)
      {
      case NEXT_CMD_REUSABLE:
      case GAME_LOSE:
      case GAME_WIN:
        return true;
      }
      return false;
    case CMD_CRTRADJ:
      return false;
    case CMD_COMMNT:
      return true;
    case CMD_OBSOLT:
      return false;

    case CMD_ADIKTED:
      return false;
    case CMD_UNKNOWN:
      return true;
    default:
      return false;
    }
}

char *recompose_script_command(const struct DK_SCRIPT_COMMAND *cmd,const struct SCRIPT_OPTIONS *optns)
{
    if (cmd==NULL) return strdup("");
    const char *cmd_name=script_cmd_text(cmd->group,cmd->index,rem_cmdtext);
    int len;
    len=(cmd->level*optns->level_spaces)+strlen(cmd_name)+2;
    int param_idx;
    for (param_idx=0;param_idx<cmd->param_count;param_idx++)
    {
       len+=strlen(cmd->params[param_idx])+2;
    }
    len+=2;
    char *text=(char *)malloc(len);
    if (text==NULL)
    {
        message_error("recompose_script_command: cannot allocate memory");
        return strdup("");
    }
    text[0]='\0';
    if (len<192)
    {
       int i,k;
       for (i=0;i<cmd->level;i++)
       {
         for (k=0;k<optns->level_spaces;k++)
           strcat(text," ");
       }
    }
    strcat(text,cmd_name);
    short no_bracket=is_no_bracket_command(cmd->group,cmd->index);
    if (no_bracket)
    {
       if (cmd->param_count>0)
         strcat(text," ");
    } else
    {
       strcat(text,"(");
    }
    int last_param=cmd->param_count-1;
    for (param_idx=0;param_idx<last_param;param_idx++)
    {
       renew_cmd_param(cmd,param_idx,optns);
       strcat(text,cmd->params[param_idx]);
       if ((param_idx>0)&&(cmd->group==CMD_CONDIT)&&
          ((cmd->index==COND_IF)||(cmd->index==IF_AVAILABLE)))
         strcat(text," ");
       else
         strcat(text,",");
    }
    if (last_param>=0)
    {
        renew_cmd_param(cmd,last_param,optns);
        strcat(text,cmd->params[last_param]);
    }
    if (!no_bracket)
        strcat(text,")");
    return text;
}

/*
 * Renews the script parameter; returns true if it was renewed, false if left
 * without changes.
 */
short renew_cmd_param(const struct DK_SCRIPT_COMMAND *cmd, const unsigned int param_idx,
    __attribute__((unused)) const struct SCRIPT_OPTIONS *optns)
{
  if ((cmd == NULL) || (param_idx >= cmd->param_count))
      return false;
  char *wordtxt=cmd->params[param_idx];
  int par_idx;
  int par_group;
  par_group=recognize_script_word_group_and_idx(&par_idx,wordtxt,true);
  if (par_idx<0)
      return false;
  const char *nword=script_cmd_text(par_group,par_idx,wordtxt);
  wordtxt=strdup(nword);
  if (wordtxt==NULL)
      return false;
  free(cmd->params[param_idx]);
  cmd->params[param_idx]=wordtxt;
  return true;
}

const char *script_cmd_text(const int group,const int cmdidx,const char *prev_val)
{
    switch (group)
    {
    /* Parameters */
    case CMD_COMP:
      return comp_plyr_cmd_text(cmdidx);
    case CMD_PLAYER:
      return players_cmd_text(cmdidx);
    case CMD_OPERATR:
      return operator_cmd_text(cmdidx);
    case CMD_OBJTYPE:
      return objtype_cmd_text(cmdidx);
    case CMD_VARIBL:
      return variabl_cmd_text(cmdidx);
    case CMD_TIMER:
      return timer_cmd_text(cmdidx);
    case CMD_FLAG:
      return flag_cmd_text(cmdidx);
    case CMD_PAROBJ:
      return party_objectv_cmd_text(cmdidx);
    case CMD_CREATR:
      return creatures_cmd_text(cmdidx);
    case CMD_DOOR:
      return door_cmd_text(cmdidx);
    case CMD_TRAP:
      return trap_cmd_text(cmdidx);
    case CMD_SPELL:
      return spell_cmd_text(cmdidx);
    case CMD_ROOM:
      return room_cmd_text(cmdidx);
    case CMD_SPECIAL:
      return special_cmd_text(cmdidx,prev_val);
    /* Commands */
    case CMD_CONDIT:
      return condit_cmd_text(cmdidx);
    case CMD_PARTY:
      return party_cmd_text(cmdidx);
    case CMD_AVAIL:
      return avail_cmd_text(cmdidx);
    case CMD_CUSTOBJ:
      return custobj_cmd_text(cmdidx);
    case CMD_SETUP:
      return setup_cmd_text(cmdidx);
    case CMD_TRIGER:
      return triger_cmd_text(cmdidx);
    case CMD_CRTRADJ:
      return crtradj_cmd_text(cmdidx);
    case CMD_COMMNT:
      return commnt_cmd_text(cmdidx);
    case CMD_OBSOLT:
      return obsolt_cmd_text(cmdidx);
    /* Special/adikted specific */
    case CMD_ADIKTED:
      return adikted_cmd_text(cmdidx);
    case CMD_UNKNOWN:
    default:
        return prev_val;
    }
}

int adikted_cmd_arrsize()
{
  return sizeof(cmd_adikted_arr)/sizeof(char *);
}

int adikted_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_adikted_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_adikted_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *adikted_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_adikted_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_adikted_arr[cmdidx];
}

int condit_cmd_arrsize()
{
  return sizeof(cmd_condit_arr)/sizeof(char *);
}

int condit_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_condit_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_condit_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *condit_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_condit_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_condit_arr[cmdidx];
}

int flag_cmd_arrsize()
{
  return sizeof(cmd_flag_arr)/sizeof(char *);
}

int flag_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_flag_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_flag_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *flag_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_flag_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_flag_arr[cmdidx];
}

int party_objectv_cmd_arrsize()
{
  return sizeof(cmd_party_objectv_arr)/sizeof(char *);
}

int party_objectv_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_party_objectv_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_party_objectv_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *party_objectv_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_party_objectv_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_party_objectv_arr[cmdidx];
}

int party_cmd_arrsize()
{
  return sizeof(cmd_party_arr)/sizeof(char *);
}

int party_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_party_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_party_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *party_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_party_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_party_arr[cmdidx];
}

int avail_cmd_arrsize()
{
  return sizeof(cmd_avail_arr)/sizeof(char *);
}

int avail_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_avail_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_avail_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *avail_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_avail_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_avail_arr[cmdidx];
}

int comp_plyr_cmd_arrsize()
{
  return sizeof(cmd_comp_plyr_arr)/sizeof(char *);
}

int comp_plyr_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_comp_plyr_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_comp_plyr_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *comp_plyr_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_comp_plyr_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_comp_plyr_arr[cmdidx];
}

int players_cmd_arrsize()
{
  return sizeof(cmd_players_arr)/sizeof(char *);
}

int players_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_players_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_players_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *players_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_players_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_players_arr[cmdidx];
}

int creatures_cmd_arrsize()
{
  return sizeof(cmd_creatures_arr)/sizeof(char *);
}

int creatures_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_creatures_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_creatures_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *creatures_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_creatures_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_creatures_arr[cmdidx];
}

int room_cmd_arrsize()
{
  return sizeof(cmd_rooms_arr)/sizeof(char *);
}

int room_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_rooms_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_rooms_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *room_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_rooms_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_rooms_arr[cmdidx];
}

int spell_cmd_arrsize()
{
  return sizeof(cmd_spells_arr)/sizeof(char *);
}

int spell_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_spells_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_spells_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *spell_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_spells_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_spells_arr[cmdidx];
}

const char *spell_cmd_fullname(int cmdidx)
{
    int array_count=sizeof(cmd_spells_fullname)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "BAD";
    return cmd_spells_fullname[cmdidx];
}

int trap_cmd_arrsize()
{
  return sizeof(cmd_traps_arr)/sizeof(char *);
}

int trap_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_traps_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_traps_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *trap_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_traps_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_traps_arr[cmdidx];
}

const char *trap_cmd_fullname(int cmdidx)
{
    int array_count=sizeof(cmd_traps_fullname)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "BAD";
    return cmd_traps_fullname[cmdidx];
}

int door_cmd_arrsize()
{
  return sizeof(cmd_doors_arr)/sizeof(char *);
}

int door_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_doors_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_doors_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *door_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_doors_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_doors_arr[cmdidx];
}

const char *door_cmd_fullname(int cmdidx)
{
    int array_count=sizeof(cmd_doors_fullname)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "BAD";
    return cmd_doors_fullname[cmdidx];
}

int special_cmd_arrsize()
{
  return 2;
}

int special_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<1)) return -1;
    if (stricmp(random_cmdtext,cmdtext)==0)
        return SPEC_RANDOM;
    int val;
    if (script_param_to_int(&val,cmdtext))
        return SPEC_NUMBER;
    return -1;
}

const char *special_cmd_text(int cmdidx,const char *cmdtext)
{
    switch (cmdidx)
    {
    case SPEC_RANDOM:
        return random_cmdtext;
    case SPEC_NUMBER:
    default:
        return cmdtext;
    }
}

int operator_cmd_arrsize()
{
  return sizeof(cmd_operator_arr)/sizeof(char *);
}

int operator_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<1)) return -1;
    int i=0;
    int array_count=sizeof(cmd_operator_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_operator_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *operator_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_operator_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_operator_arr[cmdidx];
}

int objtype_cmd_arrsize()
{
  return sizeof(cmd_objtype_arr)/sizeof(char *);
}

int objtype_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<1)) return -1;
    int i=0;
    int array_count=sizeof(cmd_objtype_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_objtype_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *objtype_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_objtype_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_objtype_arr[cmdidx];
}

int variabl_cmd_arrsize()
{
  return sizeof(cmd_variabl_arr)/sizeof(char *);
}

int variabl_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_variabl_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_variabl_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *variabl_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_variabl_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_variabl_arr[cmdidx];
}

int timer_cmd_arrsize()
{
  return sizeof(cmd_timer_arr)/sizeof(char *);
}

int timer_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_timer_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_timer_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *timer_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_timer_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_timer_arr[cmdidx];
}

int custobj_cmd_arrsize()
{
  return sizeof(cmd_custobj_arr)/sizeof(char *);
}

int custobj_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_custobj_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_custobj_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *custobj_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_custobj_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_custobj_arr[cmdidx];
}

int setup_cmd_arrsize()
{
  return sizeof(cmd_setup_arr)/sizeof(char *);
}

int setup_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_setup_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_setup_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *setup_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_setup_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_setup_arr[cmdidx];
}

int triger_cmd_arrsize()
{
  return sizeof(cmd_triger_arr)/sizeof(char *);
}

int triger_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_triger_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_triger_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *triger_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_triger_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_triger_arr[cmdidx];
}

int crtradj_cmd_arrsize()
{
  return sizeof(cmd_crtradj_arr)/sizeof(char *);
}

int crtradj_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_crtradj_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_crtradj_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *crtradj_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_crtradj_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_crtradj_arr[cmdidx];
}

int obsolt_cmd_arrsize()
{
  return sizeof(cmd_obsolt_arr)/sizeof(char *);
}

int obsolt_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_obsolt_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_obsolt_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *obsolt_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_obsolt_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_obsolt_arr[cmdidx];
}

int commnt_cmd_arrsize()
{
  return sizeof(cmd_commnt_arr)/sizeof(char *);
}

int commnt_cmd_index(const char *cmdtext)
{
    if (cmdtext==NULL) return EMPTYLN;
    if (strlen(cmdtext)<1) return EMPTYLN;
    int i=0;
    int array_count=sizeof(cmd_commnt_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_commnt_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *commnt_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_commnt_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_commnt_arr[cmdidx];
}

int orient_cmd_arrsize()
{
  return sizeof(cmd_orient_arr)/sizeof(char *);
}

int orient_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_orient_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_orient_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *orient_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_orient_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_orient_arr[cmdidx];
}

int font_cmd_arrsize()
{
  return sizeof(cmd_font_arr)/sizeof(char *);
}

int font_cmd_index(const char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_font_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (stricmp(cmd_font_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *font_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_font_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return rem_cmdtext;
    return cmd_font_arr[cmdidx];
}

/*
 * Creates new DK_SCRIPT_COMMAND and initializes its values
 */
struct DK_SCRIPT_COMMAND *script_command_create(void)
{
  struct DK_SCRIPT_COMMAND *cmd=(struct DK_SCRIPT_COMMAND *)malloc(sizeof(struct DK_SCRIPT_COMMAND));
  script_command_clear(cmd);
  return cmd;
}

/*
 * Clears values in given DK_SCRIPT_COMMAND without deallocating memory
 */
void script_command_clear(struct DK_SCRIPT_COMMAND *cmd)
{
    cmd->group=CMD_UNKNOWN;
    cmd->index=-1;
    cmd->params=NULL;
    cmd->param_count=0;
}

/*
 * Frees memory allocated for given DK_SCRIPT_COMMAND
 */
void script_command_free(struct DK_SCRIPT_COMMAND *cmd)
{
    int i;
    for (i=cmd->param_count-1;i>=0;i--)
        free(cmd->params[i]);
    free(cmd);
}

/*
 * Frees memory allocated inside given DK_SCRIPT_COMMAND and clears it
 * allowing to be used again. If there is no DK_SCRIPT_COMMAND allocated,
 * the function creates a new one.
 */
void script_command_renew(struct DK_SCRIPT_COMMAND **cmd)
{
    if ((*cmd)==NULL)
    {
      (*cmd)=script_command_create();
      return;
    }
    int i;
    for (i=(*cmd)->param_count-1;i>=0;i--)
        free((*cmd)->params[i]);
    script_command_clear(*cmd);
}

/*
 * Frees all lines in text file given in parameters
 */
void text_file_free(char **lines,int lines_count)
{
     int i;
     for (i=lines_count-1;i>=0;i--)
       free(lines[i]);
     free(lines);
}

/*
 * Adds a text line at end of given text file. Adds directly,
 * without making copy of the text. Returns index of the new line.
 */
int text_file_line_add(char ***lines,int *lines_count,char *text)
{
    int line_idx=*lines_count;
    *lines=realloc(*lines,(line_idx+1)*sizeof(char *));
    if ((*lines)==NULL)
    {
        message_error("Cannot allocate text file lines");
        return -1;
    }
    (*lines)[line_idx]=text;
    (*lines_count)++;
    return line_idx;
}

/*
 * Adds a text line at end of given text file. Adds copy of text
 * variable. Returns index of the new line.
 */
int text_file_linecp_add(char ***lines,int *lines_count,char *text)
{
    int line_idx=*lines_count;
    *lines=realloc(*lines,(line_idx+1)*sizeof(char *));
    if ((*lines)==NULL)
    {
        message_error("Cannot allocate text file lines");
        return -1;
    }
    (*lines)[line_idx]=strdup(text);
    (*lines_count)++;
    return line_idx;
}

short add_stats_to_script(char ***lines,int *lines_count,struct LEVEL *lvl)
{
    char *line;
    char *tmp;
    char *tmp2;
    char *tmp3;
    long max_len=LINEMSG_SIZE;
    /* Secure against very long descriptions */
    if (lvl->info.desc_text!=NULL)
        max_len=max(max_len,strlen(lvl->info.desc_text)+(LINEMSG_SIZE>>1));
    if ((lvl->info.author_text!=NULL)&&(lvl->info.editor_text!=NULL))
        max_len=max(max_len,strlen(lvl->info.author_text)+strlen(lvl->info.editor_text)+(LINEMSG_SIZE>>1));
    line=(char *)malloc(max_len*sizeof(char));
    tmp=(char *)malloc(max_len*sizeof(char));
    time_t curr_time=time(NULL);
    /* Script header */
    tmp2=prepare_short_fname(lvl->savfname,24);
    sprintf(line,"%s %s script file for %s",rem_cmdtext,PROGRAM_NAME,tmp2);
    free(tmp2);
    text_file_linecp_add(lines,lines_count,line);
    strftime(tmp,LINEMSG_SIZE/2, "%d %b %Y, %H:%M:%S", localtime(&curr_time) );
    sprintf(line,"%s %s %s",rem_cmdtext,"Automatically generated on",tmp);
    text_file_linecp_add(lines,lines_count,line);
    text_file_linecp_add(lines,lines_count,"");
    /* Map version */
    sprintf(line,"%s(%d,%d,%d)",levversion_cmdtext,lvl->info.ver_major,
        lvl->info.ver_minor,lvl->info.ver_rel);
    text_file_linecp_add(lines,lines_count,line);
    /* Map creation date */
    sprintf(line,"%s(%llu,%llu)",leveltimestmp_cmdtext,
        (unsigned long long)lvl->info.creat_date,(unsigned long long)curr_time);
    text_file_linecp_add(lines,lines_count,line);
    /* User commands count */
    sprintf(line,"%s(%lu,%lu,%lu,%lu)",usrcmnds_count_cmdtext,lvl->info.usr_cmds_count,
        lvl->info.usr_mdswtch_count,lvl->info.usr_slbchng_count,lvl->info.usr_creatobj_count);
    text_file_linecp_add(lines,lines_count,line);
    /* Level name - obsolete due to LIF file support */
/*    tmp2=get_lif_name_text(lvl);
    if (tmp2==NULL) tmp2="";
    sprintf(line,"%s(\"%s\")",adikted_cmd_text(LEVEL_NAME),tmp2);
    text_file_linecp_add(lines,lines_count,line);*/
    /* Level description */
    tmp2=lvl->info.desc_text;
    if (tmp2==NULL) tmp2="";
    sprintf(line,"%s(\"%s\")",adikted_cmd_text(LEVEL_DESC),tmp2);
    text_file_linecp_add(lines,lines_count,line);
    /* Level authors */
    tmp2=lvl->info.author_text;
    if (tmp2==NULL) tmp2="";
    tmp3=lvl->info.editor_text;
    if (tmp3==NULL) tmp3="";
    sprintf(line,"%s(\"%s\",\"%s\")",adikted_cmd_text(LEVEL_AUTHORS),tmp2,tmp3);
    text_file_linecp_add(lines,lines_count,line);
    free(tmp);
    free(line);
    return true;
}

short add_graffiti_to_script(char ***lines,int *lines_count,struct LEVEL *lvl)
{
    int i,k;
    struct DK_GRAFFITI *graf;
    char *line;
    line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    char *graftxt;
    graftxt=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    for (i=0; i < lvl->graffiti_count; i++)
    {
      graf = lvl->graffiti[i];
      if (graf==NULL) continue;
      int graf_len=strlen(graf->text);
      for (k=0;k<graf_len;k++)
      {
        char c=graf->text[k];
        if (c<=32) graftxt[k]='_'; else
        if (c==',') graftxt[k]='.'; else
        if (c=='(') graftxt[k]='{'; else
        if (c==')') graftxt[k]='}'; else
          graftxt[k]=c;
      }
      graftxt[graf_len]='\0';
      sprintf(line,"%s(%d,%d,%d,%s,%s,0x%03x,%s)",graffiti_cmdtext,graf->tile.x,graf->tile.y,graf->height,
                orient_cmd_text(graf->orient),font_cmd_text(graf->font),graf->cube,graftxt);
      text_file_linecp_add(lines,lines_count,line);
    }
    free(line);
    free(graftxt);
    return true;
}

short add_custom_clms_to_script(char ***lines,int *lines_count,struct LEVEL *lvl)
{
    int sx,sy;
    char *line;
    line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    for (sy=0; sy < lvl->subsize.y; sy++)
      for (sx=0; sx < lvl->subsize.x; sx++)
      {
        struct DK_CUSTOM_CLM *cclm;
        struct COLUMN_REC *clm_rec;
        cclm=get_cust_col(lvl,sx,sy);
        if (cclm!=NULL)
          clm_rec = cclm->rec;
        else
          clm_rec = NULL;
        if (clm_rec!=NULL)
        {
          sprintf(line,"%s(%d,%d,%d,%d,%d,0x%03x,0x%03x,0x%03x,0x%03x,0x%03x,0x%03x,0x%03x,0x%03x,0x%03x)",
              custom_column_cmdtext,sx,sy,
              cclm->wib_val,clm_rec->lintel,clm_rec->orientation,clm_rec->base,
              clm_rec->c[0],clm_rec->c[1],clm_rec->c[2],clm_rec->c[3],
              clm_rec->c[4],clm_rec->c[5],clm_rec->c[6],clm_rec->c[7]);
          text_file_linecp_add(lines,lines_count,line);
        }
      }
    free(line);
    return true;
}

/*
 * Returns orientation name string
 */
char *get_orientation_shortname(unsigned short orient)
{
     int types_count=sizeof(cmd_orient_shortnames)/sizeof(char *);
     if (orient<types_count)
       return (char *)cmd_orient_shortnames[orient];
     else
       return "unkn";
}

/**
 * Returns orientation constant after the given one.
 * @param orient Previous orientation constant.
 * @return Returns next orientation constant.
 */
unsigned short get_orientation_next(unsigned short orient)
{
     int array_count=sizeof(orient_constants)/sizeof(unsigned short);
    /*find the constant in proper array */
    int idx=arr_ushort_pos(orient_constants,orient,array_count);
    if ((idx<0)||(idx+1>=array_count))
        idx=0;
    else
        idx++;
    return orient_constants[idx];
}

/**
 * Returns font name string for given font index.
 * @param font Font index.
 * @return Returns font name string.
 */
char *get_font_longname(unsigned short font)
{
     int types_count=sizeof(cmd_font_fullnames)/sizeof(char *);
     if (font<types_count)
       return (char *)cmd_font_fullnames[font];
     else
       return "unkn";
}

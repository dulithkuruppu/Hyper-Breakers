#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "weapon.h"


#define RELOAD_CHANNEL 0
#define SHOOT_CHANNEL 1
#define EFFECT_CHANNEL 2
#define PLAYER_EFFECT_CHANNEL 3


typedef struct {
    Mix_Music* theme_music;
    Mix_Music* menu_music;
    Mix_Chunk* shotgun_shot;
    Mix_Chunk* rifle_shot;
    Mix_Chunk* rpg_shot;
    Mix_Chunk* sniper_shot;
    Mix_Chunk* pistol_shot;
    Mix_Chunk* out_of_ammo_effect;
    Mix_Chunk* reload_effect;
    Mix_Chunk* running_effect;
    Mix_Chunk* jump_effect;
    Mix_Chunk* gun_pickup_effect;
    Mix_Chunk* ladder_climbing_effect;
    Mix_Chunk* got_shot_effect;
    // Mix_Chunk* on_spike_effect;
    Mix_Chunk* bounce_pad_effect;
    Mix_Chunk* died_effect;

} SoundBank;

void init_audio();
void play_pistol_shot();
void play_shotgun_shot(); 
void play_rifle_shot();
void play_sniper_shot();
void play_rpg_shot();
void out_of_ammo_effect();
void reload_effect();
void running_effect();
void jump_effect();
void gun_pickup_effect();
void ladder_climbing_effect();
void got_shot_effect();
void on_spike_effect();
void bounce_pad_effect();
void died_effect();
void play_game_theme_song();
void play_game_menu_song();
void play_relevant_gun_sound (GunClass gun);
void play_relevant_theme_sound (ScreenType previous);
void free_sounds();


#endif
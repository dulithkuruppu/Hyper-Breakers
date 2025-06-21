#include "globalConsts.h"
#include "menu.h"
#include "audio.h"
#include <stdio.h>

SoundBank sounds;

// Description: Loads sound from path
Mix_Chunk* loadSound(const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (!sound) {
        fprintf(stderr, "Failed to load sound %s: %s\n", path, Mix_GetError());
    }
    return sound;
}

Mix_Music* loadMusic(const char* path) {
    Mix_Music* music = Mix_LoadMUS(path);
    if (!music) {
        fprintf(stderr, "Failed to load music %s: %s\n", path, Mix_GetError());
    }
    return music;
}

// Description: loads the audio for each sound effect
void init_audio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    Mix_AllocateChannels(8);

    sounds.theme_music = loadMusic("assets/sounds/background/theme_song.mp3");
    sounds.menu_music = loadMusic("assets/sounds/background/menu_song.mp3");
    Mix_VolumeMusic(MIX_MAX_VOLUME * 0.3);
    // jumpSound    = Mix_LoadWAV("assets/sounds/jump.wav");
    sounds.pistol_shot = loadSound("assets/sounds/shoot/pistol_shot.wav");
    sounds.shotgun_shot = loadSound("assets/sounds/shoot/shotgun_shot.wav");
    sounds.rifle_shot = loadSound("assets/sounds/shoot/rifle_shot.wav");
    sounds.sniper_shot = loadSound("assets/sounds/shoot/sniper_shot.wav");
    sounds.rpg_shot = loadSound("assets/sounds/shoot/rpg_shot.wav");
    sounds.out_of_ammo_effect = loadSound("assets/sounds/effects/out_of_ammo_effect.wav");
    sounds.reload_effect = loadSound("assets/sounds/effects/reload_effect.wav");
    sounds.running_effect = loadSound("assets/sounds/effects/running_effect.wav");
    sounds.jump_effect = loadSound("assets/sounds/effects/jump_effect.wav");
    sounds.gun_pickup_effect = loadSound("assets/sounds/effects/gun_pickup_effect.wav");
    sounds.ladder_climbing_effect = loadSound("assets/sounds/effects/climbing_ladder_effect.wav");
    sounds.got_shot_effect = loadSound("assets/sounds/effects/got_shot_effect.wav");
    // sounds.on_spike_effect = loadSound("assets/sounds/effects/on_spike_effect.wav");
    sounds.bounce_pad_effect = loadSound("assets/sounds/effects/bounce_pad_effect.wav");
    sounds.died_effect = loadSound("assets/sounds/effects/died_effect.wav");
    
    // Mix_Volume(PLAYER_EFFECT_CHANNEL, MIX_MAX_VOLUME);
}

void play_pistol_shot() {
    Mix_PlayChannel(SHOOT_CHANNEL, sounds.pistol_shot, 0);
}

void play_shotgun_shot() {
    Mix_PlayChannel(SHOOT_CHANNEL, sounds.shotgun_shot, 0);
}

void play_rifle_shot() {
    Mix_PlayChannel(SHOOT_CHANNEL, sounds.rifle_shot, 0);
}

void play_sniper_shot() {
    Mix_PlayChannel(SHOOT_CHANNEL, sounds.sniper_shot, 0);
}

void play_rpg_shot() {
    Mix_PlayChannel(SHOOT_CHANNEL, sounds.rpg_shot, 0);
}

void out_of_ammo_effect() {
    Mix_PlayChannel(EFFECT_CHANNEL, sounds.out_of_ammo_effect, 0);
}

void reload_effect() {
    Mix_PlayChannel(RELOAD_CHANNEL, sounds.reload_effect, 0);
}

void running_effect() {
    if (!sounds.running_effect) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayChannel(PLAYER_EFFECT_CHANNEL, sounds.running_effect, 0);
}

void jump_effect() {
    Mix_PlayChannel(5, sounds.jump_effect, 0);
}

void gun_pickup_effect() {
    Mix_PlayChannel(RELOAD_CHANNEL, sounds.gun_pickup_effect, 0);
}

void ladder_climbing_effect() {
    Mix_PlayChannel(6, sounds.ladder_climbing_effect, 0);
}

void got_shot_effect() {
    Mix_PlayChannel(6, sounds.got_shot_effect, 0);
}

// void on_spike_effect() {
//     Mix_PlayChannel(RELOAD_CHANNEL, sounds.on_spike_effect, 0);
// }

void bounce_pad_effect() {
    Mix_PlayChannel(RELOAD_CHANNEL, sounds.bounce_pad_effect, 0);
}

void died_effect() {
    Mix_PlayChannel(7, sounds.died_effect, 0);
}

void play_game_theme_song() {
    Mix_PlayMusic(sounds.theme_music, -1);
}

void play_game_menu_song() {
    Mix_PlayMusic(sounds.menu_music, -1);
}

void play_relevant_gun_sound (GunClass gun) {
    switch (gun) {
        case PISTOL :
            play_pistol_shot();
            break;
        case RIFLE : 
            play_rifle_shot();
            break;
        case SHOTGUN :
            play_shotgun_shot();
            break;
        case SNIPER : 
            play_sniper_shot();
            break;
        case RPG : 
            play_rpg_shot();
            break;
    }
}

// Description: Plays background music based on screen

void play_relevant_theme_sound(ScreenType previous) {
    // Toggle between GAME <-> PAUSE
    if (previous == GAME_SCREEN && cur_screen == PAUSE_SCREEN) {
        Mix_PauseMusic();            // Pause game music
        return;                      // Don't play other music
    }

    if (previous == PAUSE_SCREEN && cur_screen == GAME_SCREEN) {
        Mix_ResumeMusic();           // Resume game music from where it left off
        return;
    }

    // For other screen transitions, stop all and play appropriate music
    Mix_HaltMusic();

    if (cur_screen == GAME_SCREEN) {
        play_game_theme_song();      // Start fresh game music
    // } else if (cur_screen == S && previous == GAME_OVER_SCREEN) {
    //     return;
    } else {
        play_game_menu_song();       // Start pause/menu music
    }
}




void free_sounds() {
    Mix_FreeMusic(sounds.theme_music);
    Mix_FreeMusic(sounds.menu_music);
    Mix_FreeChunk(sounds.shotgun_shot);
    Mix_FreeChunk(sounds.rifle_shot);
    Mix_FreeChunk(sounds.rpg_shot);
    Mix_FreeChunk(sounds.sniper_shot);
    Mix_FreeChunk(sounds.pistol_shot);
    Mix_FreeChunk(sounds.out_of_ammo_effect);
    Mix_FreeChunk(sounds.reload_effect);
    Mix_FreeChunk(sounds.running_effect);
    Mix_FreeChunk(sounds.jump_effect);
    Mix_FreeChunk(sounds.gun_pickup_effect);
    Mix_FreeChunk(sounds.ladder_climbing_effect);
    Mix_FreeChunk(sounds.got_shot_effect);
    // Mix_FreeChunk(sounds.on_spike_effect);
    Mix_FreeChunk(sounds.bounce_pad_effect);
    Mix_FreeChunk(sounds.died_effect);
}




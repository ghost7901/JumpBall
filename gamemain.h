#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#pragma once

bool GameLoop();
bool GameInitiate();
bool ResourceInit();
void GameRelease();

bool DoCommands(int id);
bool IniLoad();
bool IniSave();
#endif
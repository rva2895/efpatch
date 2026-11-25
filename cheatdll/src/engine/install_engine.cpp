#include "stdafx.h"

void install_object();
void install_map();
void install_pathing();
void install_action();

void install_engine()
{
    install_object();
    install_map();
    install_pathing();
    install_action();
}

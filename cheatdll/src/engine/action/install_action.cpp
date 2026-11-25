#include "stdafx.h"

void install_RGE_Action_List();
void install_RGE_Action();
void install_RGE_Action_Enter();

// the action-runtime subsystem installer - grows per class as the
// RGE_Action_* family lands (the install_object.cpp precedent)
void install_action()
{
    install_RGE_Action_List();
    install_RGE_Action();
    install_RGE_Action_Enter();
}

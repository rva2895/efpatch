#include "stdafx.h"

void install_RGE_Action_List();
void install_RGE_Action();
void install_RGE_Action_Enter();
void install_RGE_Action_Move_To();
void install_RGE_Action_Guard();
void install_RGE_Action_Bird();
void install_RGE_Action_Make();
void install_RGE_Action_Explore();
void install_RGE_Action_Transport();
void install_RGE_Action_Gather();

// the action-runtime subsystem installer - grows per class as the
// RGE_Action_* family lands (the install_object.cpp precedent)
void install_action()
{
    install_RGE_Action_List();
    install_RGE_Action();
    install_RGE_Action_Enter();
    install_RGE_Action_Move_To();
    install_RGE_Action_Guard();
    install_RGE_Action_Bird();
    install_RGE_Action_Make();
    install_RGE_Action_Explore();
    install_RGE_Action_Transport();
    install_RGE_Action_Gather();
}

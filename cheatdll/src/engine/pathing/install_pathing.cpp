#include "stdafx.h"

// pathing/install_pathing.cpp - the pathing subsystem installer dispatcher.
// Subsystem: pathing (ratified 2026-11-19).
// Chain: install_engine() -> install_pathing() -> the per-class installers.
void install_BMotion();

void install_pathing()
{
    install_BMotion();
}

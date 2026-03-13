#include "stdafx.h"
#include "recseek.h"

TDialogPanel* SeekDialog__SeekDialog(TDialogPanel* this_)
{
    UNREFERENCED_PARAMETER(this_);

    TDialogPanel* dialog = (TDialogPanel*)operator_new_internal(sizeof(TDialogPanel));
    return dialog;
}

void setRecSeekHooks()
{

}

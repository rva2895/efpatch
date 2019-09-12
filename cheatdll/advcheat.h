#pragma once

void scanChat();
void restoreAll();

void setAdvCheatHooks();

void __stdcall takeControl(int p);

class sel_iterator //returns NULL when points at end
{
private:
	void* player;
	int index;
public:
	sel_iterator(void*);

	sel_iterator& operator++();
	sel_iterator& operator=(const sel_iterator&);
	bool operator==(const sel_iterator&);
	bool operator!=(const sel_iterator&);
	UNIT* operator*();
};

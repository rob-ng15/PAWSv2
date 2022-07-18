/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// in_mc1.c -- mouse input for MC1

#include "quakedef.h"
#include "mc1.h"

static short s_old_mouse_x;
static short s_old_mouse_y;
static unsigned s_old_mouse_buttons;

void IN_Init (void)
{
	unsigned int pos = GET_MMIO (MOUSEPOS);
	s_old_mouse_x = (short)pos;
	s_old_mouse_y = (short)(pos >> 16);

	s_old_mouse_buttons = GET_MMIO (MOUSEBTNS);
}

void IN_Shutdown (void)
{
}

void IN_Commands (void)
{
	int i;
	unsigned new_mouse_buttons = GET_MMIO (MOUSEBTNS);
	for (i = 0; i < 3; ++i)
	{
		if ((new_mouse_buttons & (1 << i)) && !(s_old_mouse_buttons & (1 << i)))
		{
			Key_Event (K_MOUSE1 + i, true);
		}
		if (!(new_mouse_buttons & (1 << i)) && (s_old_mouse_buttons & (1 << i)))
		{
			Key_Event (K_MOUSE1 + i, false);
		}
	}
	s_old_mouse_buttons = new_mouse_buttons;
}

void IN_Move (usercmd_t *cmd)
{
	unsigned int pos = GET_MMIO (MOUSEPOS);
	short new_mouse_x = (short)pos;
	short new_mouse_y = (short)(pos >> 16);
	float mouse_x = (float)(new_mouse_x - s_old_mouse_x);
	float mouse_y = (float)(new_mouse_y - s_old_mouse_y);
	s_old_mouse_x = new_mouse_x;
	s_old_mouse_y = new_mouse_y;
	int mouse_look;

	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

	// add mouse X/Y movement to cmd
	if ((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;

	// Hack: We always enable mouse look ("modern" Q3 style).
	// mouse_look = ((in_mlook.state & 1) != 0);
	mouse_look = 1;

	if (mouse_look)
		V_StopPitchDrift ();

	if (mouse_look && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}
}

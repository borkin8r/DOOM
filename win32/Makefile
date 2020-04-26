################################################################
#
# $Id:$
#
# $Log:$
#

CC=cl  # gcc or g++

CFLAGS=/DWIN32# -DUSEASM /Wall 
LDFLAGS=-L/usr/X11R6/lib
LIBS=-lXext -lX11 -lnsl -lm

# subdirectory for objects
O=..\build\win32

WDEPS=C:\

# not too sophisticated dependency
OBJS=				\
		{$(O)\}doomdef.obj		\
		{$(O)\}doomstat.obj		\
		{$(O)\}dstrings.obj		\
		{$(O)\}i_system.obj		\
		{$(O)\}i_sound.obj		\
		{$(O)\}i_video.obj		\
		{$(O)\}i_net.obj			\
		{$(O)\}tables.obj			\
		{$(O)\}f_finale.obj		\
		{$(O)\}f_wipe.obj 		\
		{$(O)\}d_main.obj			\
		{$(O)\}d_net.obj			\
		{$(O)\}d_items.obj		\
		{$(O)\}g_game.obj			\
		{$(O)\}m_menu.obj			\
		{$(O)\}m_misc.obj			\
		{$(O)\}m_argv.obj  		\
		{$(O)\}m_bbox.obj			\
		{$(O)\}m_fixed.obj		\
		{$(O)\}m_swap.obj			\
		{$(O)\}m_cheat.obj		\
		{$(O)\}m_random.obj		\
		{$(O)\}am_map.obj			\
		{$(O)\}p_ceilng.obj		\
		{$(O)\}p_doors.obj		\
		{$(O)\}p_enemy.obj		\
		{$(O)\}p_floor.obj		\
		{$(O)\}p_inter.obj		\
		{$(O)\}p_lights.obj		\
		{$(O)\}p_map.obj			\
		{$(O)\}p_maputl.obj		\
		{$(O)\}p_plats.obj		\
		{$(O)\}p_pspr.obj			\
		{$(O)\}p_setup.obj		\
		{$(O)\}p_sight.obj		\
		{$(O)\}p_spec.obj			\
		{$(O)\}p_switch.obj		\
		{$(O)\}p_mobj.obj			\
		{$(O)\}p_telept.obj		\
		{$(O)\}p_tick.obj			\
		{$(O)\}p_saveg.obj		\
		{$(O)\}p_user.obj			\
		{$(O)\}r_bsp.obj			\
		{$(O)\}r_data.obj			\
		{$(O)\}r_draw.obj			\
		{$(O)\}r_main.obj			\
		{$(O)\}r_plane.obj		\
		{$(O)\}r_segs.obj			\
		{$(O)\}r_sky.obj			\
		{$(O)\}r_things.obj		\
		{$(O)\}w_wad.obj			\
		{$(O)\}wi_stuff.obj		\
		{$(O)\}v_video.obj		\
		{$(O)\}st_lib.obj			\
		{$(O)\}st_stuff.obj		\
		{$(O)\}hu_stuff.obj		\
		{$(O)\}hu_lib.obj			\
		{$(O)\}s_sound.obj		\
		{$(O)\}z_zone.obj			\
		{$(O)\}info.obj				\
		{$(O)\}sounds.obj 		\
		{$(O)\}win32_layer.obj \
		{$(O)\}win32_doom.obj

all:	 $(O)/win32doom

clean:
	@del /f *.obj
	@rmdir /s ..\build\win32

$(O)/win32doom:	makebuilddir $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(O)/i_main.obj \
	/o $(O)/win32doom.exe $(LIBS)

$(OBJS): $*.c
	$(CC) $(CFLAGS) /Fo "$(O)\$*.obj" /c $*.c 

main: win32_doom.c
	$(CC) $(CFLAGS) /DEBUG win32_doom.c d_main.c win32_layer.c user32.lib /link /out:"$(O)\win32doom.exe" 

makebuilddir:
	if not exist "$(O)" @mkdir "$(O)"

run:
	$(O)\win32doom.exe

#############################################################
# 
#############################################################
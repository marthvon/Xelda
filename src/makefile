CC=gcc
CFLAGS =-Wall -g
OBJ= main.o \
	core/engine.o core/visual_server.o core/global_store.o core/logger.o core/timer.o core/input_handler.o core/filesystem.o \
	components/collision_map.o components/entity.o components/map.o \
	game_objects/entity/custom_entity.o game_objects/entity/enemy_spider.o game_objects/entity/statue.o game_objects/entity/arrow_entity.o \
	game_objects/entity/background.o game_objects/entity/item_node.o game_objects/entity/player.o game_objects/entity/treasure.o \
	game_objects/entity/bomb_entity.o game_objects/entity/door_entity.o
EXEC=../bin/Xelda

Xelda: $(OBJ)
	$(CC) -ffreestanding -Wl,--start-group $(OBJ) -Wl,--end-group -o $(EXEC) -Wl,-Bstatic -lSDL2_image -lSDL2 \
		-lpng16 -ltiff -lzstd -llzma -ljbig -lz -lwebp -ljpeg -ldeflate \
		-ldrm -lrt \
		-lXinerama -lXi -lXxf86vm -lXext -lXcursor -lXrandr -lXrender -lXfixes -lXss -lX11 -lxcb -lXau -lXdmcp \
		-Wl,-Bdynamic -Wl,--as-needed -lasound -Wl,--as-needed -lm -Wl,--as-needed -lpulse-simple -pthread -Wl,--as-needed -lpulse -pthread \
		-Wl,--as-needed -lgbm -Wl,--as-needed -lwayland-egl -Wl,--as-needed -lwayland-client -Wl,--as-needed -lwayland-cursor -Wl,--as-needed -lxkbcommon -Wl,--as-needed -ldecor-0 

main.o: main.c core/engine.h
	$(CC) $(CFLAGS) -c main.c -o main.o


core/engine.o: core/engine.c core/engine.h   globals.h core/visual_server.h core/global_store.h core/timer.h core/input_handler.h core/filesystem.h components/map.h components/entity.h
	$(CC) $(CFLAGS) -c core/engine.c -o core/engine.o

core/visual_server.o: core/visual_server.c core/visual_server.h   globals.h components/entity.h components/map.h game_objects/textures/texture_table.h game_objects/textures/filepath_table.h
	$(CC) $(CFLAGS) -c core/visual_server.c -o core/visual_server.o

core/logger.o: core/logger.c core/logger.h   globals.h
	$(CC) $(CFLAGS) -c core/logger.c -o core/logger.o

core/global_store.o: core/global_store.c core/global_store.h   globals.h components/entity.h game_objects/maps/events_table.h
	$(CC) $(CFLAGS) -c core/global_store.c -o core/global_store.o

core/timer.o: core/timer.c core/timer.h
	$(CC) $(CFLAGS) -c core/timer.c -o core/timer.o

core/input_handler.o: core/input_handler.c core/input_handler.h   globals.h
	$(CC) $(CFLAGS) -c core/input_handler.c -o core/input_handler.o

core/filesystem.o: core/filesystem.c core/filesystem.h   globals.h
	$(CC) $(CFLAGS) -c core/filesystem.c -o core/filesystem.o


components/map.o: components/map.c components/map.h    globals.h core/visual_server.h core/global_store.h core/filesystem.h components/entity.h components/collision_map.h game_objects/maps/map_table.h game_objects/entity/collision_table.h
	$(CC) $(CFLAGS) -c components/map.c -o components/map.o

components/entity.o: components/entity.c components/entity.h   globals.h components/map.h game_objects/entity/entity_table.h game_objects/textures/texture_table.h
	$(CC) $(CFLAGS) -c components/entity.c -o components/entity.o

components/collision_map.o: components/collision_map.c components/collision_map.h   globals.h components/map.h game_objects/entity/collision_table.h
	$(CC) $(CFLAGS) -c components/collision_map.c -o components/collision_map.o

#game_objects/entity/custom_entity.o: game_objects/entity/custom_entity.c components/entity.h   globals.h components/map.h game_objects/entity/background.h game_objects/entity/player.h game_objects/entity/entity_table.h game_objects/textures/texture_table.h
#	$(CC) $(CFLAGS) -c game_objects/entity/custom_entity.c -o game_objects/entity/custom_entity.o
#
#game_objects/entity/background.o: game_objects/entity/background.c game_objects/entity/background.h   globals.h core/visual_server.h components/entity.h components/map.h components/collision_map.h game_objects/entity/player game_objects/entity/collision_table.h game_objects/textures/texture_table.h game_objects/textures/spritesheet/spider.h
#	$(CC) $(CFLAGS) -c game_objects/entity/background.c -o game_objects/entity/background.o
#
#game_objects/entity/player.o: game_objects/entity/player.c game_objects/entity/player.h   globals.h core/input_handler.h core/visual_server.h components/entity.h components/map.h components/collision_map.h game_objects/entity/collision_table.h game_objects/textures/spritesheet/player.h
#	$(CC) $(CFLAGS) -c game_objects/entity/player.c -o game_objects/entity/player.o
#
#game_objects/entity/enemy_spider.o: game_objects/entity/enemy_spider.c game_objects/entity/enemy_spider.h
#	$(CC) $(CFLAGS) -c game_objects/entity/enemy_spider.c -o game_objects/entity/enemy_spider.o
#
#game_objects/entity/statue.o: game_objects/entity/statue.c game_objects/entity/statue.h   globals.h core/visual_server.h core/global_store.h components/entity.h components/map.h components/collision_map.h game_objects/entity/collision_table.h game_objects/entity/player.h game_objects/maps/map_table.h game_objects/maps/events_table.h game_objects/textures/texture_table.h
#	$(CC) $(CFLAGS) -c game_objects/entity/statue.c -o game_objects/entity/statue.o

clean:
	rm -rf $(OBJ) $(EXEC)
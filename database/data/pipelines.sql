INSERT INTO PIPELINES (NAME, TYPE) VALUES
	('debug_line', 1);

INSERT INTO GRAPHIC_PIPELINE_SETTINGS (PIPELINE_ID, VERTEX_SHADER, FRAGMENT_SHADER) VALUES
	(1, readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.vert.spv'), readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.frag.spv'));

INSERT INTO PIPELINE_BINDING_SETTINGS (PIPELINE_ID, BINDING_NAME, NO_AUTO_BUFFER) VALUES
	(1, 'time_info', 0),
	(1, 'screen_info', 0),
	(1, 'camera_info', 0);
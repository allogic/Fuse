INSERT INTO PIPELINES (NAME, TYPE) VALUES
	(
		'debug_line',
		0
	);

INSERT INTO GRAPHIC_PIPELINE_SETTINGS (PIPELINE_ID, VERTEX_SHADER, FRAGMENT_SHADER, VERTEX_SHADER_SIZE, FRAGMENT_SHADER_SIZE) VALUES
	(
		0,
		readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.vert.spv'),
		readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.frag.spv'),
		(OCTET_LENGTH(readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.vert.spv'))),
		(OCTET_LENGTH(readfile('C:/Users/mialb/Downloads/fuse/shader/debug/line.frag.spv')))
	);

INSERT INTO GRAPHIC_PIPELINE_VERTEX_INPUTS (PIPELINE_ID, TYPE) VALUES
	(
		0,
		0
	);
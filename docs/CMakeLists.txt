project (robosim_docs)

cmake_minimum_required (VERSION 2.8)

include (UseLATEX.cmake)

add_latex_document (robosim.tex
	IMAGE_DIRS pictures
	USE_INDEX
	DEFAULT_PDF
)

add_custom_command (TARGET pdf POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/robosim.pdf" "${CMAKE_SOURCE_DIR}"
	COMMENT "Copying robosim.pdf to source directory." )

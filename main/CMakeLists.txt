idf_component_register(SRCS "speaker_main.c"
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ""
                    REQUIRES bt driver nvs_flash
                    )

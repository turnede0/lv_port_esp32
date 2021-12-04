idf_component_register(SRCS "testpage_ui.c" "main.c" "https_ota.c" "blink.c" "gui.c" "img_icon_png.c" "adc_event.c"
                    INCLUDE_DIRS "." "./assets"
                    # Embed the server root certificate into the final binary
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
                    


                    
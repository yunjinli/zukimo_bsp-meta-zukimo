
# Dependencies and settings for all images go here

# include a few image post-process task definitions
# that are commonly used.
inherit zukimo-common-tasks

ROOTFS_POSTPROCESS_COMMAND:append = " add_to_rootfs_image;"
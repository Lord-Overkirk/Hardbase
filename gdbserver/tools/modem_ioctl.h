#define IOCTL_MODEM_OFF _IO('o', 0x20)
#define IOCTL_MODEM_RESET _IO('o', 0x21)
#define IOCTL_MODEM_STATUS _IO('o', 0x27)


enum modem_state {
    STATE_OFFLINE,
    STATE_CRASH_RESET,	/* silent reset */
    STATE_CRASH_EXIT,	/* cp ramdump */
    STATE_BOOTING,
    STATE_ONLINE,
    STATE_NV_REBUILDING,	/* <= rebuilding start */
    STATE_LOADER_DONE,
    STATE_SIM_ATTACH,
    STATE_SIM_DETACH,
    STATE_CRASH_WATCHDOG,	/* cp watchdog crash */
};

struct sipc_fmt_hdr {
	u_int16_t len;
	u_int8_t  msg_seq;
	u_int8_t  ack_seq;
	u_int8_t  main_cmd;
	u_int8_t  sub_cmd;
	u_int8_t  cmd_type;
} __packed;
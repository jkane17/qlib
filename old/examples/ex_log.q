
.qlib.module.load `log;

logAll:{[]
    .log.trace "I am a TRACE message";
    .log.debug "I am a DEBUG message";
    .log.info "I am an INFO message";
    .log.warn "I am a WARN message";
    .log.error "I am an ERROR message";
    .log.fatal "I am a FATAL message";
 };

// By default, log level is INFO so DEBUG and TRACE messages are not logged
logAll[];

// Let's log everything
.log.setLvl `TRACE;

// We now get DEBUG and TRACE messages
logAll[];

// Turn of logging
.log.setLvl `OFF;

// This does nothing
logAll[];

// Back to default
.log.setLvl `INFO;

.log.info "I have meta information";

// Turn off meta logging
.log.disableMeta[];
.log.info "I do not have meta information";

// Turn on meta logging
.log.enableMeta[];
.log.info "I have meta information again!";

// What is the current log level?
.log.getLvl[]

// Try to set a "bad" level
@[.log.setLvl;`BAD;{x}]

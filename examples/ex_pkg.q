
// Load the log package (and any of its dependencies)
loaded:.qlib.module.load `log;

.log.info "Loaded packages: ",csv sv string loaded;

.log.info:{};

// Oh no, I have accidently redefined the .log.info function
.log.info "IMPORTANT MESSAGE!";

// I can get the original definition back by reloading the package
// Note : does not reload the dependencies
reloaded:.pkg.reload `log;

// Ah, that's better
.log.info "Reloaded packages: ",csv sv string reloaded;

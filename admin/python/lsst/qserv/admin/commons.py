import io
import os
import hashlib
import getpass
import logging
import re
import subprocess
import sys
import ConfigParser


NO_STATUS_SCRIPT = -1
DOWN = 255
UP = 0

config = dict()

log = logging.getLogger(__name__)

def read_user_config():
    global log
    config_file = os.path.join(os.getenv("HOME"), ".lsst", "qserv.conf")
    log.debug("Read user config file: %s", config_file)
    config = read_config(config_file)
    return config


def read_config(config_file):

    global config, log
    log.debug("Reading config file : %s" % config_file)
    if not os.path.isfile(config_file):
        log.fatal("qserv configuration file not found : %s" % config_file)
        exit(1)

    parser = ConfigParser.SafeConfigParser()
    # parser.readfp(io.BytesIO(const.DEFAULT_CONFIG))
    parser.read(config_file)

    log.debug("Build configuration : ")
    for section in parser.sections():
        log.debug("===")
        log.debug("[%s]" % section)
        log.debug("===")
        config[section] = dict()
        for option in parser.options(section):
            log.debug("'%s' = '%s'" % (option, parser.get(section, option)))
            config[section][option] = parser.get(section, option)

    # normalize directories names
    for section in config.keys():
        for option in config[section].keys():
            if re.match(".*_dir", option):
                config[section][option] = os.path.normpath(
                    config[section][option])
    # computable configuration parameters
    config['qserv']['scratch_dir'] = os.path.join("/dev", "shm", "qserv-%s-%s" %
                                                  (getpass.getuser(),
                                                   hashlib.sha224(config['qserv']['run_base_dir']).hexdigest())
                                                  )

    # TODO : manage special characters for pass (see config file comments for
    # additional information)
    config['mysqld']['pass'] = parser.get("mysqld", "pass", raw=True)
    if parser.has_option('mysqld', 'port'):
        config['mysqld']['port'] = parser.getint('mysqld', 'port')

    config['mysql_proxy']['port'] = parser.getint('mysql_proxy', 'port')

    return config


def getConfig():
    return config

def restart(service_name):
    config = getConfig()
    if len(config) == 0:
        raise RuntimeError("Qserv configuration is empty")
    initd_path = os.path.join(config['qserv']['run_base_dir'], 'etc', 'init.d')
    daemon_script = os.path.join(initd_path, service_name)
    out = os.system("%s stop" % daemon_script)
    out = os.system("%s start" % daemon_script)

def status(qserv_run_dir):
    """
    Check if Qserv services are up
    @qserv_run_dir Qserv run directory of Qserv instance to ckeck
    @return: the exit code of qserv-status.sh, i.e.:
             id status file doesn't exists: -1 (NO_STATUS_SCRIPT)
             if all Qserv services are up:   0 (UP)
             if all Qserv services are down: 255 (DOWN)
             else the number of stopped Qserv services
    """
    script_path = os.path.join(qserv_run_dir, 'bin', 'qserv-status.sh')
    if not os.path.exists(script_path):
        return NO_STATUS_SCRIPT
    with open(os.devnull, "w") as fnull:
        retcode = subprocess.call([script_path], stdout=fnull, stderr=fnull, shell=False)
    return retcode

def run_command(cmd_args, stdin_file=None, stdout=None, stderr=None,
                loglevel=logging.INFO):
    """
    Run a shell command
    @cmdargs  command arguments
    @stdin    can be a filename, or None
    @stdout   can be sys.stdout, a filename, or None
              which redirect to current processus output
    @stderr   same as stdout
    @loglevel print stdin, stdout and stderr if current module logger
              verbosity is greater than loglevel
    """
    global log

    cmd_str = ' '.join(cmd_args)
    log.log(loglevel, "Run shell command: {0}".format(cmd_str))

    sin = None
    if stdin_file:
        log.log(loglevel, "stdin file : %s" % stdin_file)
        sin = open(stdin_file, "r")

    sout = None
    if stdout==sys.stdout:
        sout=sys.stdout
    elif stdout:
        log.log(loglevel, "stdout file : %s" % stdout)
        sout = open(stdout, "w")
    else:
        sout = subprocess.PIPE

    serr = None
    if stderr==sys.stderr:
        serr=sys.stderr
    elif stderr:
        log.log(loglevel, "stderr file : %s" % stderr)
        serr = open(stderr, "w")
    else:
        serr = subprocess.PIPE

    try:
        process = subprocess.Popen(
            cmd_args, stdin=sin, stdout=sout, stderr=serr
        )

        (stdoutdata, stderrdata) = process.communicate()

        if stdoutdata != None and len(stdoutdata) > 0:
            log.info("\tstdout :\n--\n%s--" % stdoutdata)
        if stderrdata != None and len(stderrdata) > 0:
            log.info("\tstderr :\n--\n%s--" % stderrdata)

        if process.returncode != 0:
            log.fatal(
                "Error code returned by command : {0} ".format(cmd_str))
            sys.exit(1)

    except OSError as e:
        log.fatal("Error : %s while running command : %s" %
                     (e, cmd_str))
        sys.exit(1)
    except ValueError as e:
        log.fatal("Invalid parameter : '%s' for command : %s " %
                     (e, cmd_str))
        sys.exit(1)

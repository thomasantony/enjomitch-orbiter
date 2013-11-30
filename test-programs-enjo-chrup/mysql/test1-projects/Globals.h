#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#define CREATE_WORKERS _("CREATE TABLE IF NOT EXISTS workers (wid INT NOT NULL AUTO_INCREMENT,"\
                        "surname TEXT CHARACTER SET utf8 NOT NULL, name TEXT CHARACTER SET utf8,"\
                        " city TEXT CHARACTER SET utf8, PRIMARY KEY(wid)) ENGINE = InnoDB;")

#define CREATE_PROJECTS _("CREATE TABLE IF NOT EXISTS projects (pid INTEGER NOT NULL AUTO_INCREMENT, "\
                        "name TEXT CHARACTER SET utf8 NOT NULL,deadline DATE, revenue INTEGER, PRIMARY KEY (pid))"\
                        "ENGINE = InnoDB;")

#define CREATE_ASSIGNMENTS _("CREATE TABLE IF NOT EXISTS assignments (wid INTEGER, pid INTEGER,"\
                        "FOREIGN KEY (wid) REFERENCES workers(wid) ON DELETE CASCADE ON UPDATE "\
                        "CASCADE, FOREIGN KEY (pid) REFERENCES projects(pid) ON DELETE CASCADE "\
                        "ON UPDATE CASCADE) ENGINE = InnoDB;")

#endif // GLOBALS_H_INCLUDED

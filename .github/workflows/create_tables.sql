CREATE TABLE init_scripts (
    id serial PRIMARY KEY,
    name TEXT,
    content TEXT
);

CREATE TABLE execute_scripts (
    id serial PRIMARY KEY,
    name TEXT,
    content TEXT
);

CREATE TABLE builds (
    id serial PRIMARY KEY,
    name TEXT,
    init_script_id INTEGER,
    execute_script_id INTEGER,
    FOREIGN KEY (init_script_id) REFERENCES init_scripts(id),
    FOREIGN KEY (execute_script_id) REFERENCES execute_scripts(id)
);


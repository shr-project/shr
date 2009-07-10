CREATE TABLE call_ids (
	id INTEGER PRIMARY KEY,
	number TEXT
);

CREATE INDEX call_ids_id_idx ON call_ids (id);

CREATE TABLE call_events (
	id INTEGER,
	status INTEGER,
	eventTime TIMESTAMP,
	FOREIGN KEY (id) REFERENCES call_ids(id)
);

CREATE INDEX call_events_id_idx ON call_events (id);

CREATE TRIGGER insert_call_events_eventTime AFTER  INSERT ON call_events
BEGIN
	UPDATE call_events SET eventTime = DATETIME('NOW')  WHERE rowid = new.rowid;
END;

CREATE VIEW calls AS
SELECT e.id, 
       call_ids.number, 
       e.status AS direction, 
       e.eventTime AS startTime, 
       act.eventTime AS activeTime, 
       rel.eventTime AS releaseTime, 
       strftime("%s", rel.eventTime)-strftime("%s", act.eventTime) AS duration
FROM call_events AS e
       INNER JOIN call_ids ON e.id = call_ids.id
       LEFT JOIN  call_events AS act ON e.id=act.id and act.status=2
       INNER JOIN call_events AS rel ON e.id=rel.id and rel.status=4
WHERE e.status in (0,1) 
GROUP BY e.id;

	
CREATE VIEW missed_calls AS
SELECT id, number, direction, startTime FROM calls WHERE activeTime IS NULL;

CREATE VIEW due_calls AS
SELECT id, number, startTime
FROM calls
WHERE
	direction = 0 AND activeTime IS NULL
	AND id NOT IN (SELECT id FROM calls WHERE direction = 1 AND activeTime IS NOT NULL);

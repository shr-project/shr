CREATE TABLE call_ids (
	id INTEGER PRIMARY KEY,
	number TEXT
);

CREATE TABLE call_events (
	id INTEGER,
	status INTEGER,
	eventTime TIMESTAMP,
	FOREIGN KEY (id) REFERENCES call_ids(id)
);

CREATE TRIGGER insert_call_events_eventTime AFTER  INSERT ON call_events
BEGIN
	UPDATE call_events SET eventTime = DATETIME('NOW')  WHERE rowid = new.rowid;
END;

CREATE VIEW calls AS
SELECT 
	id,
	
	(SELECT number
		FROM call_ids
		WHERE call_ids.id = call_events.id
	)
	AS number,
	
	(SELECT status
		FROM call_events as e
		WHERE e.id = call_events.id
			AND (e.status = 0 OR e.status = 1)
	)
	AS direction,
	
	(SELECT eventTime
		FROM call_events as e
		WHERE e.id = call_events.id
			AND (e.status = 0 OR e.status = 1)
	)
	AS startTime,
	
	(SELECT eventTime
		FROM call_events as e
		WHERE e.id = call_events.id
			AND e.status = 2
	)
	AS activeTime,
	
	(SELECT eventTime
		FROM call_events as e
		WHERE e.id = call_events.id
			AND e.status = 4
	)
	AS releaseTime,
	
	strftime('%M:%S',strftime('%s',(SELECT eventTime
		FROM call_events as e
		WHERE e.id = call_events.id
			AND e.status = 4
	)) - strftime('%s',(SELECT eventTime
		FROM call_events as e
		WHERE e.id = call_events.id
			AND e.status = 2
	)), "unixepoch")
	AS duration
	
FROM call_events
GROUP BY id;

CREATE VIEW missed_calls AS
SELECT id, number, direction, startTime FROM calls WHERE activeTime IS NULL;

CREATE VIEW due_calls AS
SELECT id, number, startTime
FROM calls
WHERE
	direction = 0 AND activeTime IS NULL
	AND id NOT IN (SELECT id FROM calls WHERE direction = 1 AND activeTime IS NOT NULL);

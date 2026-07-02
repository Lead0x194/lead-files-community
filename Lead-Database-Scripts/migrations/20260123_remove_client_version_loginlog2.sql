-- Remove the obsolete client_version column from log.loginlog2.
-- DB-qualified + IF EXISTS so the migration is idempotent and safe to re-run
-- (the column is already absent in the current base/log.sql dump, and lead-db-setup
-- replays unseen migrations on every install/upgrade).
alter table log.loginlog2 drop column if exists client_version;

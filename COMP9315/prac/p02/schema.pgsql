drop type if exists SchemaTuple cascade;
create type SchemaTuple as ("table" text, "attributes" text);

create or replace function schema1() returns setof SchemaTuple
as $$
declare 
        rec record;
        rel text := '';
        att text := '';
        atts text := '';
        out SchemaTuple;
	    len integer := 0;
begin
	for rec in
		select relname,attname,typname
		from   pg_class t
        join pg_attribute a on (t.oid = a.attrelid)
        join pg_namespace n on (t.relnamespace = n.oid)
        join pg_type ty on (ty.oid = a.atttypid)
		where  t.relkind='r'
			and n.nspname = 'public'
			and attnum > 0
		order by relname,attnum
	loop
		if (rec.relname <> rel) then
			if (rel <> '') then
				--out := rel || '(' || att || ')';
                out."table" := rel;
                out.attributes := atts;
				return next out;
			end if;
			rel := rec.relname;
			atts := '';
			len := 0;
		end if;
		if (atts <> '') then
			atts := atts || ', ';
			len := len + 2;
		end if;
		if (len + length(rec.attname) > 70) then
			atts := atts || E'\n        ';
			len := 0;
		end if;

        att := rec.attname||':'||rec.typname;

		atts := atts || att;
		len := len + length(att);
	end loop;
	-- deal with last table
	if (rel <> '') then
		out.table := rel;
        out.attributes := atts;
		return next out;
	end if;
end;
$$ language plpgsql;

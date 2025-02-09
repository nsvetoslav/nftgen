drop table if exists nftgen.GeneratedNfts;

drop schema if exists nftgen;
create schema nftgen;

create table nftgen.GeneratedNfts
(
    id BIGSERIAL PRIMARY KEY not null,
	metadata_uuid "uuid" not null,
	directory_id bigint not null,
	directory text not null,
	trait_id bigint not null,
	trait_name text not null
);

-- select
-- 	*
-- from nftgen.GeneratedNfts


DROP SCHEMA IF EXISTS bds_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS bds_schema;

CREATE TABLE IF NOT EXISTS bds_schema.orders (
                                                 order_id BIGSERIAL PRIMARY KEY,
                                                 weight INTEGER DEFAULT(1) NOT NULL,
    regions INTEGER DEFAULT(1) NOT NULL,
    cost INTEGER DEFAULT(1) NOT NULL,
    delivery_hours TEXT[] NOT NULL,
    completed_time TIMESTAMP
    );

CREATE TABLE IF NOT EXISTS bds_schema.couriers (
                                                   courier_id BIGSERIAL PRIMARY KEY,
                                                   regions INTEGER[],
                                                   courier_type VARCHAR(10),
    working_hours TEXT[]
    CONSTRAINT courier_type CHECK (courier_type IN ('FOOT', 'AUTO', 'BIKE'))
    );

CREATE TABLE IF NOT EXISTS bds_schema.completed_orders (
                                                           courier_id INTEGER NOT NULL,
                                                           order_id INTEGER NOT NULL,
                                                           complete_date TIMESTAMP NOT NULL,
                                                           Constraint PK_order_courier Primary Key (courier_id, order_id)
    );
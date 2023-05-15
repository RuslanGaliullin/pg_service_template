# enrollment_template

Template of a C++ service that uses [userver framework](https://github.com/userver-framework/userver) with PostgreSQL.


## Download and Build

To create your own userver-based service follow the following steps:

1. Press the green "Use this template button" at the top of this github page
2. Clone the service `git clone your-service-repo && cd your-service-repo`
3. Give a propper name to your service and replace all the occurences of "enrollment_template" string with that name
   (could be done via `find . -not -path "./third_party/*" -not -path ".git/*" -not -path './build_*' -type f | xargs sed -i 's/enrollment_template/YOUR_SERVICE_NAME/g'`).
4. Feel free to tweak, adjust or fully rewrite the source code of your service.


## Makefile

Makefile contains typicaly useful targets for development:

* `make docker-build-debug` - debug build of the service with all the assertions and sanitizers enabled in docker environment
* `make docker-test-debug` - does a `make build-debug` and runs all the tests on the result in docker environment
* `make docker-start-service-release` - does a `make install-release` and runs service in docker environment
* `make docker-start-service-debug` - does a `make install-debug` and runs service in docker environment
* `make docker-clean-data` - stop docker containers and clean database data
* `make format` - autoformat all the C++ and Python sources
* `make clean-` - cleans the object files
* `make dist-clean` - clean all, including the CMake cached configurations
* `make install` - does a `make build-release` and runs install in directory set in environment `PREFIX`
* `make install-debug` - does a `make build-debug` and runs install in directory set in environment `PREFIX`

Edit `Makefile.local` to change the default configuration and build options.


## License

The original template is distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE)
and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change
the license and CLA.

curl -X POST 'http://localhost:8080/orders' \
-H 'accept: application/json' \
-H 'Content-Type: application/json' \
-d '{
"orders": [
{
"weight": 1,
"regions": 1,
"delivery_hours": ["00:00-11:00"],
"cost": 1
},
{
"weight": 2,
"regions": 2,
"delivery_hours": ["10:00-11:00"],
"cost": 2
},
{
"weight": 3,
"regions": 3,
"delivery_hours": ["00:00-11:00"],
"cost": 3
},
{
"weight": 4,
"regions": 4,
"delivery_hours": ["23:59-11:00"],
"cost": 4
}
]
}'

curl -X POST 'http://localhost:8080/couriers' \
-H 'accept: application/json' \
-H 'Content-Type: application/json' \
-d '{
"couriers": [
{
"courier_type": "FOOT",
"regions": [0, 1, 2],
"working_hours": ["08:00-12:00", "14:00-18:00"]
},
{
"courier_type": "CAR",
"regions": [3, 4],
"working_hours": ["09:00-17:00"]
},
{
"courier_type": "BIKE",
"regions": [0, 2, 4],
"working_hours": ["10:00-14:00", "16:00-20:00"]
}
]
}'
import aiohttp
import asyncio
from typing import List
import logging
import config
from bs4 import BeautifulSoup


class LocationResult:
    def __init__(self, name, country, city, street, house_number, postcode, lat, lon):
        self.name = name
        self.country = country
        self.city = city
        self.street = street
        self.house_number = house_number
        self.postcode = postcode
        self.lat = lat
        self.lon = lon


class PlacesResponse:
    def __init__(self, features):
        self.features = features


class PlaceDescriptionResponse:
    def __init__(self, wikipedia_extracts):
        self.wikipedia_extracts = wikipedia_extracts


class WeatherResponse:
    def __init__(self, main):
        self.main = main


async def search_locations(session: aiohttp.ClientSession, query: str) -> List[LocationResult]:
    url = f"https://graphhopper.com/api/1/geocode?q={query}&locale=ru&key={config.GRAPHOPPER_API_KEY}"
    async with session.get(url) as resp:
        if resp.status != 200:
            logging.error(f"Invalid response status: {resp.status}")
            raise Exception(f"Invalid response status: {resp.status}")

        content_type = resp.headers.get('Content-Type', '')
        if not content_type.startswith('application/json'):
            logging.error(f"Invalid content type: {content_type}")
            raise Exception(f"Invalid content type: {content_type}")

        result = await resp.json()
        locations = []
        for hit in result.get('hits', []):
            locations.append(LocationResult(
                name=hit.get('name'),
                country=hit.get('country'),
                city=hit.get('city'),
                street=hit.get('street'),
                house_number=hit.get('housenumber'),
                postcode=hit.get('postcode'),
                lat=hit.get('point', {}).get('lat'),
                lon=hit.get('point', {}).get('lng')
            ))
        return locations


async def get_place_description(session: aiohttp.ClientSession, xid: str) -> PlaceDescriptionResponse:
    url = f"https://api.opentripmap.com/0.1/en/places/xid/{xid}?apikey={config.OPENTRIPMAP_API_KEY}"

    async with session.get(url) as resp:
        if resp.status != 200:
            logging.error(f"Invalid response status descrs: {resp.status}")
            raise Exception(f"Invalid response status: {resp.status}")

        if not resp.headers.get('Content-Type', '').startswith('application/json'):
            logging.error(f"Invalid content type: {resp.headers.get('Content-Type')}")
            raise Exception(f"Invalid content type: {resp.headers.get('Content-Type')}")

        result = await resp.json()

        title = result.get('name', 'Название не указано')
        description = result.get('wikipedia_extracts', {}).get('text', 'Описание недоступно')
        html_description = result.get('wikipedia_extracts', {}).get('html', '')

        soup = BeautifulSoup(html_description, 'html.parser')
        clean_description = soup.get_text()

        formatted_description = (
            f"**{title}**\n"
            f"Описание: {description}\n\n"
            f"Полное описание:\n{clean_description}"
        )

        return PlaceDescriptionResponse(wikipedia_extracts=formatted_description)


async def get_weather(session: aiohttp.ClientSession, lat: float, lon: float) -> WeatherResponse:
    url = f"https://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={config.OPENWEATHERMAP_API_KEY}"
    async with session.get(url) as resp:
        if resp.status != 200:
            error_text = await resp.text()
            logging.error(f"Invalid response status: {resp.status}. Error: {error_text}")
            raise Exception(f"Invalid response status: {resp.status}. Error: {error_text}")

        content_type = resp.headers.get('Content-Type', '')
        if not content_type.startswith('application/json'):
            logging.error(f"Invalid content type: {content_type}")
            raise Exception(f"Invalid content type: {content_type}")

        result = await resp.json()
        return WeatherResponse(main=result.get('main', {}))


async def get_places(session: aiohttp.ClientSession, lat: float, lon: float, radius: int, limit: int) -> PlacesResponse:
    url = ("https://api.opentripmap.com/0.1/en/places/radius?radius={radius}&lon={lon}&lat={lat}&kinds="
           "interesting_places&limit={limit}&format=geojson&apikey={api_key}").format(
        radius=radius,
        lon=lon,
        lat=lat,
        limit=limit,
        api_key=config.OPENTRIPMAP_API_KEY
    )

    async with session.get(url) as resp:
        if resp.status != 200:
            error_text = await resp.text()
            logging.error(f"Invalid response status: {resp.status}. Error: {error_text}")
            raise Exception(f"Invalid response status: {resp.status}. Error: {error_text}")

        content_type = resp.headers.get('Content-Type', '')
        if not content_type.startswith('application/json'):
            logging.error(f"Invalid content type: {content_type}")
            raise Exception(f"Invalid content type: {content_type}")

        result = await resp.json()
        return PlacesResponse(features=result.get('features', []))


async def main():
    async with aiohttp.ClientSession() as session:
        try:
            query = input("Введите название локации: ")

            locations = await search_locations(session, query)
            if not locations:
                logging.warning("Нет найденных локаций")
                return

            print("Найдены следующие локации:")
            for i, location in enumerate(locations):
                print(f"{i + 1}. {location.name}, {location.city}, {location.country}, "
                      f"lat={location.lat}, lon={location.lon}")

            choice = int(input("Выберите номер локации: ")) - 1
            selected_location = locations[choice]

            weather = await get_weather(session, selected_location.lat, selected_location.lon)
            temperature = weather.main.get('temp')
            print(f"Температура в {selected_location.name}: {temperature - 273.15:.2f} °C")

            places = await get_places(session, selected_location.lat, selected_location.lon, 1000, 5)
            if places.features:
                print(f"Найдены интересные места в {selected_location.name}:")
                for feature in places.features:
                    xid = feature['properties']['xid']
                    description = await get_place_description(session, xid)
                    print(description.wikipedia_extracts)

            else:
                print("Не найдено интересных мест.")
        except Exception as e:
            logging.error(f"Произошла ошибка: {e}")


if __name__ == "__main__":
    asyncio.run(main())

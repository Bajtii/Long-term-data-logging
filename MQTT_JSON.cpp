#define MQTTPP_NO_CONSTEXPR
#include <iostream>
#include <string>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "mqtt/async_client.h"

using json = nlohmann::json;

const std::string SERVER_ADDRESS = "tcp://192.168.194.242:1883";
const std::string CLIENT_ID = "EnergyMonitorClient1";
const std::string TOPIC = "pscr/c3";

const std::string MQTT_USER = "pscr-mqtt-user";
const std::string MQTT_PASSWORD = "O#$0RvcmHeqN6%t*miF8";

mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void wyswietl_dane(const json& podsumowanie, int saldo) {
    std::cout << "\n** Stan systemu energetycznego **\n";
    std::cout << "Zapotrzebowanie: " << podsumowanie["zapotrzebowanie"] << " MW\n";
    std::cout << "Generacja      : " << podsumowanie["generacja"] << " MW\n";
    std::cout << "Cieplne        : " << podsumowanie["cieplne"] << " MW\n";
    std::cout << "Wodne          : " << podsumowanie["wodne"] << " MW\n";
    std::cout << "Wiatrowe       : " << podsumowanie["wiatrowe"] << " MW\n";
    std::cout << "Fotowoltaiczne : " << podsumowanie["PV"] << " MW\n";
    std::cout << "Inne odnawialne: 0 MW\n";
    std::cout << "Czestotliwosc  : " << podsumowanie["czestotliwosc"] << " Hz\n";
    std::string kierunek = (saldo >= 0) ? "EKSPORT" : "IMPORT";
    std::cout << "Saldo wymiany  : " << abs(saldo) << " MW (" << kierunek << ")\n";
    std::cout << "**********************************\n";
}

void send_to_mqtt(const std::string& message) {
    try {
        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, message);
        pubmsg->set_qos(1);
        client.publish(pubmsg)->wait();
    }
    catch (const mqtt::exception& e) {
        std::cerr << "Blad wysylania do MQTT: " << e.what() << std::endl;
    }
}

void process_data(const std::string& json_str) {
    try {
        json j = json::parse(json_str);
        auto podsumowanie = j["data"]["podsumowanie"];

        int saldoWymiany = 0;
        for (auto& przesyl : j["data"]["przesyly"]) {
            saldoWymiany += przesyl["wartosc"].get<int>();
        }

        // Przygotowanie wiadomości JSON z dodatkowymi danymi
        json message_json;
        message_json["zapotrzebowanie"] = podsumowanie["zapotrzebowanie"];
        message_json["generacja"] = podsumowanie["generacja"];
        message_json["cieplne"] = podsumowanie["cieplne"];
        message_json["wodne"] = podsumowanie["wodne"];
        message_json["wiatrowe"] = podsumowanie["wiatrowe"];
        message_json["PV"] = podsumowanie["PV"];
        message_json["czestotliwosc"] = podsumowanie["czestotliwosc"];
        message_json["saldo"] = saldoWymiany;

        // Wydrukowanie szczegółowych danych na ekran
        wyswietl_dane(podsumowanie, saldoWymiany);

        // Wysłanie wiadomości do MQTT
        std::string message = message_json.dump();
        send_to_mqtt(message);
    }
    catch (const std::exception& e) {
        std::cerr << "Blad przetwarzania danych JSON: " << e.what() << std::endl;
    }
}

void fetch_data_from_api() {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::string url = "https://www.pse.pl/transmissionMapService";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/curl/certs/cacert.pem");

        std::string cookie = "ROUTEID=.lifereay2; LFR_SESSION_STATE_20159=1743455139014";
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json, text/javascript, */*; q=0.01");
        headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL blad: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cout << "Odebrano dane z API. Dlugosc: " << response.length() << " bajtow" << std::endl;
            process_data(response);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    try {
        mqtt::connect_options conn_opts;
        conn_opts.set_clean_session(true);
        conn_opts.set_user_name(MQTT_USER);
        conn_opts.set_password(MQTT_PASSWORD);

        client.connect(conn_opts)->wait();
        std::cout << "Polaczono z brokerem MQTT" << std::endl;

        while (true) {
            fetch_data_from_api();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    catch (const mqtt::exception& e) {
        std::cerr << "Blad MQTT: " << e.what() << std::endl;
    }

    return 0;
}
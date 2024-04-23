# Watermeter LoRaWAN Integratie

## Overzicht

Dit Arduino-sketch maakt het mogelijk om een watermeter zijn metingen via LoRaWAN naar een externe bron te laten communiceren. Het maakt gebruik van een interrupt-gestuurde aanpak om pulsen van de watermeter te detecteren, de opgehoopte waterverbruiksgegevens periodiek op te slaan in een array en de gegevens over LoRaWAN te verzenden op gedefinieerde intervallen.

## Hardware Opstelling

- Sluit de pulsuitvoerpin van de watermeter aan op de gespecificeerde pin (standaard: Pin 2).
- Zorg ervoor dat de LoRaWAN-module correct is aangesloten op het Arduino-bord.

## Configuratie

### LoRaWAN Parameters

- **Device EUI (devEui):** Unieke identificatie voor het apparaat. Vervang dit door de EUI die door je LoRa-provider is verstrekt.
- **Applicatie EUI (appEui):** Identificatie voor de toepassing. Vervang dit door de EUI die door je LoRa-provider is verstrekt.
- **Applicatiesleutel (appKey):** Versleutelingssleutel voor communicatie. Vervang dit door de sleutel die door je LoRa-provider is verstrekt.

### LoRaWAN Regio en Klasse

- **Regio:** Selecteer de juiste LoRaWAN-regio in de Arduino IDE-tools.
- **Klasse:** Klasse A en Klasse C worden ondersteund. Pas aan indien nodig.

### LoRaWAN Kanaal Masker

- Pas de array `userChannelsMask` aan zoals vereist voor je toepassing.

### LoRaWAN Transmissie-instellingen

- **Duty Cycle:** Pas de waarde `appTxDutyCycle` aan indien nodig.
- **ADR (Adaptive Data Rate):** Schakel ADR in of uit met de variabele `loraWanAdr`.

### Andere Parameters

- Pas het interval voor het opslaan van gegevens (`intervalSaveData`) en verzenden via LoRaWAN (`intervalLoraSend`) aan zoals nodig.

## Gebruik

1. Stel de hardware in volgens de instructies in de "Hardware Opstelling" sectie.
2. Wijzig de LoRaWAN parameters in de sketch met de waarden die zijn verstrekt door je LoRa-provider.
3. Upload de sketch naar je Arduino-bord.
4. Controleer de seriële uitvoer voor debug- en statusberichten.
5. Zorg voor een juiste ontvangst van gegevens op de LoRaWAN-netwerkserver.

## Belangrijke Opmerking

- **Beveiliging:** Zorg ervoor dat de LoRaWAN-sleutels (devEui, appEui, appKey) vertrouwelijk worden gehouden en niet worden blootgesteld in het code-repository of tijdens de verzending.
- **Aanpassing:** Pas de code aan volgens specifieke vereisten, zoals gegevensverwerking, foutafhandeling of extra sensorintegratie.

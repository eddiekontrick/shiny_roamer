#include "GameSequence.h"
#include <Arduino.h>

void black2_title_sequence() {
    delay(6600); pressA(200);
    delay(3000); pressA(200);
    delay(2500); pressA(200);
    delay(1000); pressA(200);
    delay(2000); pressA(200);
    delay(3000); pressA(200);
    delay(2200); pressA(200);
}

void black2_encounter_sequence() {
    delay(8000); pressA(200);
    delay(1000); pressA(200);
    delay(800); pressA(200); Serial.println("[DEBUG] Encounter Pokemon");
}

void platinum_title_sequence() {
    delay(11000); pressA(200);
    delay(500); pressA(200);
    delay(1000); pressA(200);
    delay(3500); pressA(200);
    delay(3000); pressB(200);
}

void platinumStarterSequence() {
    delay(2000); pressA(200);
    delay(5444); pressA(200);
    delay(327); pressRight(100);
    delay(915); pressRight(100);
    delay(209); pressRight(100);
    delay(349); pressRight(100);
    delay(311); pressA(200);
    delay(762); pressA(200);
    delay(3026); pressA(200);
    delay(1417); pressA(200);
    delay(763); pressA(200);
    delay(1365); pressA(200);
    delay(1061); pressA(200);
    delay(1171); pressA(200);
    delay(1260); pressA(200);
    delay(960); pressA(200);
    delay(4024); pressA(200); Serial.println("Rowan and Lucas walk away pause");
    delay(1163); pressA(200);
    delay(3832); pressA(200);
    delay(1039); pressA(200);
    delay(5241); pressA(200);
    delay(1214); pressA(200);
    delay(534); pressA(200);
    delay(1165); pressA(200);
    delay(862); pressA(200);
    delay(876); pressA(200);
}

void frlg_title_sequence() {
    delay(5000); pressA(200); Serial.println("Start Encounter SEQ");
    delay(795); pressA(200);
    delay(636); pressA(200);
    delay(677); pressA(200);
    delay(449); pressA(200);
    delay(412); pressA(200);
    delay(718); pressA(200);
    delay(492); pressA(200);
    delay(472); pressA(200);
    delay(816); pressA(200); 
    delay(556); pressA(200);
    delay(392); pressA(200);
    delay(1246); pressB(200); Serial.println("Skip Recap");
    delay(161); pressB(200);
    delay(208); pressB(200);
    delay(269); pressB(200);
    delay(308); pressB(200);
}

void frlgStarterSequence() {
    delay(200); pressA(200); Serial.println("Start Cutscene");
    delay(1326); pressA(200);
    delay(1120); pressA(200);
    delay(1854); pressA(200);
    delay(1000); pressA(200);
    delay(1751); pressB(200); Serial.println("No nickname");
    delay(1911); pressB(200);
    delay(2442); pressA(200);
    delay(4147); pressStart(200);
    delay(961); pressA(200);
    delay(1202); pressA(200);
    delay(435); pressA(200);
}

void rse_title_sequence() {
    delay(6000); pressA(200);
}

void diamond_title_sequence() {
    delay(0); pressReset(109); Serial.println("Begin Title Sequence");
    delay(9665); pressA(124);
    delay(2282); pressA(94);
    delay(3145); pressA(63);
}

void encounter_giratina() {
    delay(30); pressB(94); 
    delay(1848); pressA(108);
    delay(185); pressA(46);
    delay(479); pressA(123);
}

void hgss_title_sequence(){
    delay(500);
    delay(8800); pressA(78); Serial.println("First press");
    delay(1772); pressA(122); 
    delay(3207); pressA(59); 
}

void enter_cavern() {
    delay(171); pressUp(747);
    delay(1024); pressLeft(1071);
    delay(0); pressDown(1628);
    delay(0); pressRight(822);
    delay(0); pressDown(2059);
    delay(0); pressLeft(1439);
    delay(0); pressB(7439);
    delay(0); pressUp(759);
}

void mount_bike(){
    pressY(200);
}

void route_reset_walk(){
    pressDown(1650);
    pressUp(1700);
    delay(400);
    pressDown(50);
    delay(200);
}

void roamer_cutscene(){
    delay(2800); pressLeft(125); 
    delay(14900); pressB(109); 
    delay(1303); pressB(77);
    delay(1345); pressB(108);
    delay(877); pressB(92);
    delay(2284); pressB(92);
    delay(816); pressB(93);
    delay(1343); pressB(124);
    delay(880); pressB(93);
    delay(1237); pressB(123);
    delay(883); pressB(78); 
    delay(2000); 
}

void use_escape_rope() {
    delay(200); pressX(93); 
    delay(449); pressUp(123);
    delay(234); pressUp(92);
    delay(500); pressA(61); 
    delay(1554); pressA(124);
    delay(866); pressA(92); 
}

void bike_to_route() {
    mount_bike(); Serial.print("Mount bike");
    delay(200); pressDown(700);
    delay(0); pressRight(980);
    delay(0); pressDown(2160);
    delay(1000);
    mount_bike();
    delay(300);
    use_repel_hgss();
    delay(300);
}

void use_repel_hgss() {
    Serial.println("Using repel");
    pressB(100); Serial.println("Close text box");
    delay(100);
    pressB(100);
    delay(200); pressX(105); Serial.println("Open bag");
    delay(402); pressA(154);
    delay(1381); pressA(138);
    delay(497); pressA(47);
    delay(1256); pressA(123);
    delay(434); pressB(139);
    delay(2163); pressB(139); Serial.println("Close bag");
}

void roamer_setup_sequence(){
    hgss_title_sequence();
    roamer_cutscene(); 
    use_escape_rope();
    delay(12000); 
    bike_to_route();
}

void knock_out_raikou() {
    delay(400);
    delay(0); pressA(140);
    delay(480); pressA(93);
    delay(546); pressA(109);
    delay(14256); pressB(108);
    delay(2195); pressB(124);
    delay(1726); pressB(126);
    delay(1555); pressB(76);
}

void knock_out_entei() {
    delay(600);
    delay(0); pressA(140);
    delay(154); pressA(110);
    delay(373); pressA(139);
    delay(10000); pressB(108); Serial.println("Exit battle first press");
    delay(2098); pressB(92);
    delay(1628); pressB(157);
}

void return_from_lost() {
    mount_bike();
    delay(300); pressDown(2740);
    delay(500); pressY(125);
    delay(300); pressUp(2240);
    delay(300); pressDown(62);
}

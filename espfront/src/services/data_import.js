//data_import.js
import http from "../http-common";

class EspData {
  getLog() {
    return http.get("/accesslogs");
  }

  updateKey(id, data) {
    return http.put(`/peoples/${id}/update-key`, data);
  }

  getKey() {
    return http.get("/keys");
  }

  getPeople() {
    return http.get("/peoples");
  }
}

export default new EspData();

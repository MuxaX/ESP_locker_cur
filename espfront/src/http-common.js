//http-common.js
import axios from "axios";

export default axios.create({
  baseURL: "http://192.168.0.4:8000/api",
  headers: {
    Accept: "application/json",
    "Content-Type": "application/json",
    "Access-Control-Allow-Origin": "http://localhost:8080",
    "Access-Control-Allow-Methods": "GET,PUT,POST,DELETE,PATCH,OPTIONS",
  },
  withCredentials: false,
});

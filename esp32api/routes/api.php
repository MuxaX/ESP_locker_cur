<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\AccessLogController;
use App\Http\Controllers\PeopleController;
use App\Http\Controllers\DoorController;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');

Route::post('/access', [AccessLogController::class, 'createLog']);
Route::get('/accesslogs', [AccessLogController::class, 'getLog']);
Route::get('/accesslogs/{reader}', [AccessLogController::class, 'getLogByReader']);

Route::get('/keys', [PeopleController::class, 'KeyList']);

Route::get('/peoples', [PeopleController::class, 'PeopleList']);


Route::put('/peoples/{id}/update-key', [PeopleController::class, 'updateKey']);

Route::put('/door', [DoorController::class, 'update']);


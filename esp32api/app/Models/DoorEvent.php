<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class DoorEvent extends Model
{
    use HasFactory;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
     public $timestamps = false; // Отключаем автоматические временные метки
    protected $fillable = [
        'status',    // Статус двери (строка)
        'reader',    // Номер считывателя (1 или 2)
    ];

    /**
     * Значения по умолчанию для атрибутов модели.
     *
     * @var array
     */
    protected $attributes = [
        'status' => 'Closed', // Значение по умолчанию
    ];
}
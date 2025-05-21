<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class AccessLog extends Model
{
    protected $fillable = [
        'status',
        'uid',
        'reader',
        'timestamp'
    ];

    // Приводим timestamp к Carbon-объекту
    protected $casts = [
        'timestamp' => 'datetime',
    ];

public function magazin()
{
    return $this->hasOne(\App\Models\Magazin::class, 'log_id');
}
}
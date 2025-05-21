<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class Magazin extends Model
{

    protected $table = 'magazin';
    public function people()
    {
        return $this->belongsTo(\App\Models\People::class, 'people_id');
    }

    public function accessLog()
    {
        return $this->belongsTo(AccessLog::class, 'log_id');
    }
}

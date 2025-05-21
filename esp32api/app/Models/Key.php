<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class Key extends Model
{
     protected $fillable = ['uid'];

    public function people()
    {
        return $this->hasMany(People::class);
    }
}
